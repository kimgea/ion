/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script
File:	IonScriptValidator.h
-------------------------------------------
*/

#ifndef ION_SCRIPT_VALIDATOR_H
#define ION_SCRIPT_VALIDATOR_H

#include <cassert>
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include "IonScriptError.h"
#include "IonScriptTree.h"
#include "adaptors/IonFlatMap.h"
#include "adaptors/IonFlatSet.h"
#include "types/IonTypes.h"
#include "types/IonTypeTraits.h"

namespace ion::script
{
	namespace script_validator
	{
		enum class ParameterType
		{
			Boolean,
			Color,
			Enumerable,
			FloatingPoint,
			Integer,
			String,
			Vector2,
			Vector3
		};

		enum class Ordinality : bool
		{
			Mandatory,
			Optional
		};

		enum class ClassType : bool
		{
			Abstract,
			Concrete
		};

		enum class OutputOptions
		{
			Summary,
			Errors,
			SummaryAndErrors
		};


		class ParameterDefinition;
		class PropertyDefinition;
		class ClassDefinition;

		struct PropertyDeclaration;
		class ClassDeclaration;

		using ParameterDefinitions = std::vector<ParameterDefinition>;
		using PropertyDeclarations = adaptors::FlatSet<PropertyDeclaration>;
		using ClassDeclarations = adaptors::FlatSet<ClassDeclaration>;

		template <typename T>
		using EntityType = std::variant<std::string, T>;


		namespace detail
		{
			template <typename T>
			class declaration
			{
				static_assert(std::is_same_v<T, PropertyDefinition> || std::is_same_v<T, ClassDefinition>);

				private:

					EntityType<T> entity_;
					Ordinality ordinality_ = Ordinality::Optional;

				public:

					///@brief Constructs an incomplete declaration (no definition)
					declaration(std::string name, Ordinality ordinality) noexcept :
						entity_{std::move(name)},
						ordinality_{ordinality}
					{
						//Empty
					}

					///@brief Constructs a complete declaration (full definition)
					declaration(T definition, Ordinality ordinality) noexcept :
						entity_{std::move(definition)},
						ordinality_{ordinality}
					{
						//Empty
					}


					/**
						@name Observers
						@{
					*/

					///@brief Returns true if this declaration has a definition
					inline auto HasDefinition() const noexcept
					{
						return std::holds_alternative<T>(entity_);
					}

					///@brief Returns the name from this declaration
					///@details Could be just a name or the name of the actual definition
					inline auto& Name() const noexcept
					{
						return HasDefinition() ?
							std::get<T>(entity_).Name() :
							std::get<std::string>(entity_);
					}

					///@brief Returns an immutable reference to the definition
					inline auto& Definition() const noexcept
					{
						assert(HasDefinition());
						return std::get<T>(entity_);
					}

					///@brief Returns true if this declaration is required
					inline auto Required() const noexcept
					{
						return ordinality_ == Ordinality::Mandatory;
					}

					///@}

					/**
						@name Comparators
						@{
					*/

					///@brief Returns true if this declaration should be sorted before the given declaration
					inline auto operator<(const declaration &rhs) const noexcept
					{
						return Name() < rhs.Name();
					}

					///@brief Returns true if this declaration should be sorted before the given name
					///@details This operator is only enabled for string types
					template <typename T, typename = std::enable_if_t<types::is_string_v<T>>>
					inline auto operator<(const T &rhs) const noexcept
					{
						return Name() < rhs;
					}

					///@brief Returns true if the given name should be sorted before the given declaration
					///@details This operator is only enabled for string types
					template <typename T, typename = std::enable_if_t<types::is_string_v<T>>>
					inline friend auto operator<(const T &lhs, const declaration &rhs) noexcept
					{
						return lhs < rhs.Name();
					}

					///@}
			};

			struct class_reference final
			{
				std::string_view Name;
				const ClassDefinition *Owner = nullptr;

				class_reference(std::string_view name, const ClassDefinition &owner) noexcept;
				bool operator<(const class_reference &rhs) const noexcept;
			};


			/**
				@name Searching
				@{
			*/

			template <typename T>
			struct declaration_result final
			{
				const T *Declaration = nullptr;
				const ClassDefinition *Owner = nullptr;
				int Depth = 0;

				inline auto operator<(const declaration_result &rhs) const noexcept
				{
					return Depth < rhs.Depth;
				}
			};

			using inner_class_declarations = std::vector<declaration_result<ClassDeclaration>>;
			using property_declarations = std::vector<declaration_result<PropertyDeclaration>>;
			using inner_classes_map = adaptors::FlatMap<std::string_view, inner_class_declarations>;
			using properties_map = adaptors::FlatMap<std::string_view, property_declarations>;

			struct declarations_result
			{
				inner_classes_map InnerClasses;
				properties_map Properties;
			};

			using class_pointers = std::vector<const ClassDefinition*>;
			using inhertied_class_pointers = adaptors::FlatSet<const ClassDefinition*>;


			const ClassDefinition* find_inherited_class_definition_impl(const class_pointers &classes, const class_reference &class_ref);
			const ClassDefinition* find_class_definition_impl(bool &unwinding, bool inheriting,
				const ClassDefinition &class_def, const class_reference &class_ref);
			void find_all_declarations_impl(declarations_result &result, inhertied_class_pointers &inherited_classes,
				const ClassDefinition &root, const ClassDefinition &class_def, int depth);		

			const ClassDefinition* find_inherited_class_definition(const ClassDefinition &class_def, const class_reference &class_ref);
			const ClassDefinition* find_class_definition(const ClassDefinition &root, const class_reference &class_ref);
			declarations_result find_all_declarations(const ClassDefinition &root, const ClassDefinition &class_def);


			class class_definition_cacher final
			{
				private:

					const ClassDefinition &root_;
					adaptors::FlatMap<class_reference, const ClassDefinition*> cache_;				

				public:

					class_definition_cacher(const ClassDefinition &root) noexcept;
					const ClassDefinition* Get(const class_reference &class_ref);
			};

			class class_declarations_cacher final
			{
				private:

					const ClassDefinition &root_;
					adaptors::FlatMap<const ClassDefinition*, declarations_result> cache_;

				public:

					class_declarations_cacher(const ClassDefinition &root) noexcept;
					const declarations_result& Get(const ClassDefinition &class_def);
			};

			///@}

			/**
				@name Validating
				@{
			*/

			adaptors::FlatSet<std::string_view> get_required_properties(const properties_map &properties);
			adaptors::FlatSet<std::string_view> get_required_classes(const inner_classes_map &inner_classes);

			bool validate_property(const script_tree::PropertyNode &property, const property_declarations &overload_set);
			bool validate_properties(const ScriptTree &tree, const script_tree::ObjectNode &object, const ClassDefinition &class_def,
				class_declarations_cacher &declarations_cacher, std::vector<ValidateError> &errors);
			const ClassDefinition* validate_class(const ScriptTree &tree, const script_tree::ObjectNode &object, const ClassDefinition &class_owner,
				class_definition_cacher &definition_cacher, class_declarations_cacher &declarations_cacher, std::vector<ValidateError> &errors);

			bool validate(const ScriptTree &tree, const ClassDefinition &root, ValidateError &error, std::vector<ValidateError> &errors);
			bool validate_tree(const ScriptTree &tree, const ClassDefinition &root, std::vector<ValidateError> &errors);

			///@}

			/**
				@name Outputting
				@{
			*/

			std::string print_output(duration validate_time, const std::vector<ValidateError> &errors, OutputOptions output_options);

			///@}
		} //detail


		///@brief A class representing a parameter definition in the validation scheme
		///@details The parameter (data) type, defines the rules for validating a particular parameter
		class ParameterDefinition final
		{
			private:

				ParameterType type_;
				std::optional<adaptors::FlatSet<std::string>> values_;

			public:

				///@brief Constructs a new parameter with the given type
				ParameterDefinition(ParameterType type) noexcept;

				///@brief Constructs a new enumerable parameter with the given values
				ParameterDefinition(Strings values) noexcept;

				///@brief Constructs a new enumerable parameter with the given values
				ParameterDefinition(std::initializer_list<std::string> values);


				/**
					@name Observers
					@{
				*/

				///@brief Returns the parameter type
				[[nodiscard]] inline auto Type() const noexcept
				{
					return type_;
				}

				///@}

				/**
					@name Lookup
					@{
				*/

				///@brief Returns true if an enumerable has support for the given value
				[[nodiscard]] bool HasValue(std::string_view value) const noexcept;

				///@}

				/**
					@name Ranges
					@{
				*/

				///@brief Returns an immutable range of all values for this enumerable parameter
				///@details This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Values() const noexcept
				{
					assert(type_ == ParameterType::Enumerable && values_);
					return values_->Elements();
				}

				///@}
		};

		///@brief A class representing a property definition in the validation scheme
		///@details The name, parameters and required parameter combined, defines the rules for validating a particular property
		class PropertyDefinition final
		{
			private:

				std::string name_;
				ParameterDefinitions parameters_;
				int required_parameters_ = std::size(parameters_);

			public:

				///@brief Constructs a new property with the given name and parameter
				PropertyDefinition(std::string name, ParameterDefinition parameter) noexcept;

				///@brief Constructs a new property with the given name and parameters
				PropertyDefinition(std::string name, ParameterDefinitions parameters) noexcept;

				///@brief Constructs a new property with the given name, parameters and required parameters
				PropertyDefinition(std::string name, ParameterDefinitions parameters, int required_parameters) noexcept;


				/**
					@name Observers
					@{
				*/

				///@brief Returns the name of this property
				[[nodiscard]] inline auto& Name() const noexcept
				{
					return name_;
				}

				///@brief Returns the max required parameters of this property
				[[nodiscard]] inline auto& RequiredParameters() const noexcept
				{
					return required_parameters_;
				}

				///@}

				/**
					@name Ranges
					@{
				*/

				///@brief Returns an immutable range of all parameters for this property
				///@details This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Parameters() const noexcept
				{
					return adaptors::ranges::Iterable<const ParameterDefinitions&>{parameters_};
				}

				///@}
		};

		///@brief A class representing a class definition in the validation scheme
		///@details The name, properties, base and inner classes combined, defines the rules for validating a particular class
		class ClassDefinition final
		{
			private:

				std::string name_;
				PropertyDeclarations properties_;
				ClassDeclarations base_classes_;				
				ClassDeclarations inner_classes_;

			public:

				///@brief Constructs a new class definition with the given name
				ClassDefinition(std::string name) noexcept;

				///@brief Constructs a new class definition with the given name and base class name
				ClassDefinition(std::string name, std::string base_class);

				///@brief Constructs a new class definition with the given name and base class definition
				ClassDefinition(std::string name, ClassDefinition base_class);

				///@brief Constructs a new class with the given name and base classes
				ClassDefinition(std::string name, std::initializer_list<EntityType<ClassDefinition>> base_classes);


				/**
					@name Static functions
					@{
				*/

				///@brief Returns a newly created class definition with the given name
				///@details Designed for fluent interface by using function chaining (named parameter idiom)
				[[nodiscard]] static ClassDefinition Create(std::string name) noexcept;

				///@brief Returns a newly created class definition with the given name and base class name
				///@details Designed for fluent interface by using function chaining (named parameter idiom)
				[[nodiscard]] static ClassDefinition Create(std::string name, std::string base_class);

				///@brief Returns a newly created class definition with the given name and base class definition
				///@details Designed for fluent interface by using function chaining (named parameter idiom)
				[[nodiscard]] static ClassDefinition Create(std::string name, ClassDefinition base_class);

				///@brief Returns a newly created class definition with the given name and base classes
				///@details Designed for fluent interface by using function chaining (named parameter idiom)
				[[nodiscard]] static ClassDefinition Create(std::string name, std::initializer_list<EntityType<ClassDefinition>> base_classes);

				///@}

				/**
					@name Observers
					@{
				*/

				///@brief Returns the name of this class
				[[nodiscard]] inline auto& Name() const noexcept
				{
					return name_;
				}

				///@}

				/**
					@name Classes
					@{
				*/

				///@brief Adds an inner class with the given name
				///@details Could be a class with no definition or an existing class defined in an outer scope
				ClassDefinition& AddClass(std::string name);

				///@brief Adds an inner class with the given definition
				ClassDefinition& AddClass(ClassDefinition class_def);


				///@brief Adds an abstract inner class with the given name
				///@details Could be a class with no definition or an existing class defined in an outer scope
				ClassDefinition& AddAbstractClass(std::string name);

				///@brief Adds an abstract inner class with the given definition
				ClassDefinition& AddAbstractClass(ClassDefinition class_def);


				///@brief Adds a required inner class with the given name
				///@details Could be a class with no definition or an existing class defined in an outer scope
				ClassDefinition& AddRequiredClass(std::string name);

				///@brief Adds a required inner class with the given definition
				ClassDefinition& AddRequiredClass(ClassDefinition class_def);

				///@}

				/**
					@name Properties
					@{
				*/

				///@brief Adds a property with the given name
				ClassDefinition& AddProperty(std::string name);

				///@brief Adds a property with the given name and parameter
				ClassDefinition& AddProperty(std::string name, ParameterDefinition parameter);
				
				///@brief Adds a property with the given name and parameters
				ClassDefinition& AddProperty(std::string name, ParameterDefinitions parameters);
				
				///@brief Adds a property with the given name, parameters and the required number of parameters in range [1, |parameters|]
				ClassDefinition& AddProperty(std::string name, ParameterDefinitions parameters, int required_parameters);

				///@brief Adds a property with the given property definition
				ClassDefinition& AddProperty(PropertyDefinition property);


				///@brief Adds a required property with the given name
				ClassDefinition& AddRequiredProperty(std::string name);

				///@brief Adds a required property with the given name and parameter
				ClassDefinition& AddRequiredProperty(std::string name, ParameterDefinition parameter);
				
				///@brief Adds a required property with the given name and parameters
				ClassDefinition& AddRequiredProperty(std::string name, ParameterDefinitions parameters);
				
				///@brief Adds a required property with the given name, parameters and the required number of parameters in range [1, |parameters|]
				ClassDefinition& AddRequiredProperty(std::string name, ParameterDefinitions parameters, int required_parameters);

				///@brief Adds a required property with the given property definition
				ClassDefinition& AddRequiredProperty(PropertyDefinition property);

				///@}

				/**
					@name Lookup
					@{
				*/

				///@brief Returns a pointer to an immutable ClassDeclaration with the given name
				///@details If no base class with the given name is found, it returns nullptr
				[[nodiscard]] const ClassDeclaration* GetBaseClass(std::string_view name) const noexcept;

				///@brief Returns a pointer to an immutable ClassDeclaration with the given name
				///@details If no inner class with the given name is found, it returns nullptr
				[[nodiscard]] const ClassDeclaration* GetInnerClass(std::string_view name) const noexcept;

				///@brief Returns a pointer to an immutable PropertyDeclaration with the given name
				///@details If no property with the given name is found, it returns nullptr
				[[nodiscard]] const PropertyDeclaration* GetProperty(std::string_view name) const noexcept;

				///@}

				/**
					@name Ranges
					@{
				*/

				///@brief Returns an immutable range of all base classes of this class
				///@details This can be used directly with a range-based for loop
				[[nodiscard]] inline auto BaseClasses() const noexcept
				{
					return base_classes_.Elements();
				}

				///@brief Returns an immutable range of all inner classes of this class
				///@details This can be used directly with a range-based for loop
				[[nodiscard]] inline auto InnerClasses() const noexcept
				{
					return inner_classes_.Elements();
				}

				///@brief Returns an immutable range of all properties of this class
				///@details This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Properties() const noexcept
				{
					return properties_.Elements();
				}

				///@}
		};


		///@brief A class representing a property declaration of a particular property definition
		///@details Definition and ordinality combined, defines the rules for validating a particular declaration
		struct PropertyDeclaration final : detail::declaration<PropertyDefinition>
		{
			using detail::declaration<PropertyDefinition>::declaration;
		};

		///@brief A class representing a class declaration of a particular class definition
		///@details Definition, ordinality and class type combined, defines the rules for validating a particular declaration
		class ClassDeclaration final : public detail::declaration<ClassDefinition>
		{
			private:

				ClassType class_type_ = ClassType::Concrete;

			public:

				///@brief Constructs an incomplete declaration (no definition)
				ClassDeclaration(std::string name, Ordinality ordinality, ClassType class_type) noexcept;

				///@brief Constructs a complete declaration (full definition)
				ClassDeclaration(ClassDefinition definition, Ordinality ordinality, ClassType class_type) noexcept;


				/**
					@name Observers
					@{
				*/

				///@brief Returns true if this class declaration is instantiatable
				[[nodiscard]] inline auto Instantiatable() const noexcept
				{
					return class_type_ == ClassType::Concrete;
				}

				///@}
		};
	} //script_validator


	///@brief A class that validates a compiled tree structure against a defined validation scheme
	///@details The validation scheme is defined by adding classes (with inheritance) and properties (with arguments).
	///The classes and properties combined, defines the rules for validating a particular script
	class ScriptValidator final
	{
		private:

			script_validator::ClassDefinition root_{{}};

			std::vector<ValidateError> validate_errors_;
			duration validate_time_{};
    
		public:

			ScriptValidator() = default;


			/**
				@name Static functions
				@{
			*/

			///@brief Returns a newly created script validator
			///@details Designed for fluent interface by using function chaining (named parameter idiom)
			[[nodiscard]] static ScriptValidator Create() noexcept;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns all validate errors from the previous validation
			///@details The errors returned are all validation errors found.
			///The validation is okay if no errors returned
			[[nodiscard]] inline auto& ValidateErrors() const noexcept
			{
				return validate_errors_;
			}

			///@brief Returns the validate time of the previous validation
			[[nodiscard]] inline auto ValidateTime() const noexcept
			{
				return validate_time_;
			}

			///@}

			/**
				@name Classes
				@{
			*/

			///@brief Adds a class with the given name (no definition)
			ScriptValidator& AddClass(std::string name);

			///@brief Adds a class with the given definition
			ScriptValidator& AddClass(script_validator::ClassDefinition class_def);


			///@brief Adds an abstract class with the given definition
			ScriptValidator& AddAbstractClass(script_validator::ClassDefinition class_def);


			///@brief Adds a required class with the given name (no definition)
			ScriptValidator& AddRequiredClass(std::string name);

			///@brief Adds a required class with the given definition
			ScriptValidator& AddRequiredClass(script_validator::ClassDefinition class_def);

			///@}

			/**
				@name Lookup
				@{
			*/

			///@brief Returns a pointer to an immutable ClassDeclaration with the given name
			///@details If no class with the given name is found, it returns nullptr
			[[nodiscard]] const script_validator::ClassDeclaration* GetClass(std::string_view name) const noexcept;

			///@}

			/**
				@name Outputting
				@{
			*/

			///@brief Prints the output from the previous compilation
			///@details Whats printed is based on the given compiler output options
			[[nodiscard]] std::string PrintOutput(script_validator::OutputOptions output_options = script_validator::OutputOptions::SummaryAndErrors) const;

			///@}

			/**
				@name Validating
				@{
			*/

			///@brief Returns true if this validator validates the given script tree
			[[nodiscard]] bool Validate(const ScriptTree &tree, ValidateError &error);

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns an immutable range of all classes in this validator
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Classes() const noexcept
			{
				return root_.InnerClasses();
			}

			///@}
	};

} //ion::script

#endif