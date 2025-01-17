/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script
File:	IonScriptTree.h
-------------------------------------------
*/

#ifndef ION_SCRIPT_TREE_H
#define ION_SCRIPT_TREE_H

#include <array>
#include <cassert>
#include <optional>
#include <string>
#include <variant>
#include <type_traits>
#include <vector>

#include "IonScriptTypes.h"
#include "adaptors/ranges/IonIterable.h"
#include "types/IonTypes.h"
#include "types/IonTypeTraits.h"
#include "utilities/IonStringUtility.h"

namespace ion::script
{
	namespace script_tree
	{
		/*
			Forward declaration
			Node types
		*/

		struct TreeNode;
		class ObjectNode;
		class PropertyNode;
		class ArgumentNode;

		using ObjectNodes = std::vector<ObjectNode>;
		using PropertyNodes = std::vector<PropertyNode>;
		using ArgumentNodes = std::vector<ArgumentNode>;

		using ArgumentType =
			std::variant<
				ScriptType::Boolean,
				ScriptType::Color,
				ScriptType::Enumerable,
				ScriptType::FloatingPoint,
				ScriptType::Integer,
				ScriptType::String,
				ScriptType::Vector2,
				ScriptType::Vector3>;


		enum class SearchStrategy : bool
		{
			DepthFirst,
			BreadthFirst
		};

		enum class DepthFirstTraversal : bool
		{
			PreOrder,
			PostOrder
		};

		enum class AppendCondition
		{
			Unconditionally,
			NoDuplicateNames,
			NoDuplicateClasses //Same as above for properties
		};

		enum class PrintOptions
		{
			Objects,
			Properties,
			Arguments
		};


		namespace detail
		{
			template <typename T, typename Ts>
			struct is_contained_in;

			template <typename T, template <typename...> typename U, typename ...Ts>
			struct is_contained_in<T, U<Ts...>> : std::disjunction<std::is_same<T, Ts>...> {};

			template <typename T, typename U>
			constexpr auto is_contained_in_v = is_contained_in<T, U>::value;


			enum class serialization_section_token : std::underlying_type_t<std::byte>
			{	
				Object,
				Property,
				Argument
			};

			using search_result = std::vector<TreeNode>;

			using generation = std::vector<ObjectNode*>;
			using generations = std::vector<generation>;
			using lineage_search_result = std::vector<generations>;


			/*
				Serializing
			*/

			template <typename VariantT, typename T, size_t Index = 0>
			constexpr size_t variant_index() noexcept
			{
				static_assert(Index < std::variant_size_v<VariantT>);
				
				if constexpr (std::is_same_v<std::variant_alternative_t<Index, VariantT>, T>)
					return Index;
				else
					return variant_index<VariantT, T, Index + 1>();
			}


			template <typename T>
			inline void serialize_value(const T &value, std::vector<std::byte> &bytes)
			{
				if constexpr (std::is_same_v<T, std::string>)
				{
					auto data_size = utilities::string::Serialize(std::size(value));
					auto data = reinterpret_cast<const std::byte*>(std::data(value));
					bytes.insert(std::end(bytes), std::begin(data_size), std::end(data_size));
					bytes.insert(std::end(bytes), data, data + std::size(value));
				}
				else
				{
					auto data = utilities::string::Serialize(value);
					bytes.insert(std::end(bytes), std::begin(data), std::end(data));
				}
			}
			
			template <typename T>
			inline void serialize_argument(const T &value, std::vector<std::byte> &bytes)
			{
				static_assert(is_contained_in_v<T, ArgumentType>);

				bytes.push_back(static_cast<std::byte>(serialization_section_token::Argument));
				bytes.push_back(static_cast<std::byte>(variant_index<ArgumentType, T>()));
				serialize_value(value.Get(), bytes);
			}


			template <typename T>
			inline auto deserialize_value(std::string_view bytes, T &value)
			{
				if constexpr (std::is_same_v<T, std::string>)
				{
					if (std::size(bytes) >= sizeof(size_t))
					{
						std::array<std::byte, sizeof(size_t)> output;
						std::copy(std::begin(bytes), std::begin(bytes) + std::size(output), reinterpret_cast<char*>(std::data(output)));
						
						size_t data_size;
						utilities::string::Deserialize(output, data_size);

						if (std::size(bytes) >= std::size(output) + data_size)
						{
							value = T{bytes.substr(std::size(output), data_size)};
							return std::ssize(output) + static_cast<int>(data_size);
						}
					}
				}
				else
				{
					if (std::size(bytes) >= sizeof(T))
					{		
						std::array<std::byte, sizeof(T)> output;
						std::copy(std::begin(bytes), std::begin(bytes) + std::size(output), reinterpret_cast<char*>(std::data(output)));
						utilities::string::Deserialize(output, value);
						return std::ssize(output);
					}
				}

				return 0;
			}

			template <typename T>
			inline auto deserialize_argument(std::string_view bytes, ArgumentNodes &arguments)
			{
				typename T::value_type value;
				auto bytes_deserialized = deserialize_value(bytes, value);

				std::string unit;
				auto unit_bytes_deserialized = bytes_deserialized > 0 ?
					deserialize_value(bytes.substr(bytes_deserialized), unit) : 0;

				if (unit_bytes_deserialized > 0)
					arguments.emplace_back(T{value}, unit);

				return bytes_deserialized + unit_bytes_deserialized;
			}


			void serialize_argument(const ArgumentNode &argument, std::vector<std::byte> &bytes);
			void serialize_property(const PropertyNode &property, std::vector<std::byte> &bytes);
			void serialize_object(const TreeNode &node, std::vector<std::byte> &bytes);
			std::vector<std::byte> serialize(const ObjectNodes &objects);

			int deserialize_argument(std::string_view bytes, ArgumentNodes &arguments);
			int deserialize_property(std::string_view bytes, PropertyNodes &properties, ArgumentNodes &arguments);
			int deserialize_object(std::string_view bytes, std::vector<ObjectNodes> &object_stack, PropertyNodes &properties);
			std::optional<ObjectNodes> deserialize(std::string_view bytes);


			/*
				Appending
			*/

			template <typename DestNodes, typename SrcNodes>
			void append_nodes(DestNodes &destination, const SrcNodes &source, AppendCondition append_condition)
			{
				auto count = std::size(destination); //Count, before appending

				switch (append_condition)
				{
					case AppendCondition::Unconditionally:
					{
						destination.insert(std::end(destination), std::begin(source), std::end(source));
						break;
					}

					case AppendCondition::NoDuplicateClasses:
					{
						if constexpr (std::is_same_v<std::remove_cvref_t<decltype(*std::begin(source))>, ObjectNode>)
						{
							for (auto &candidate : source)
							{
								if (auto iter = std::find_if(std::cbegin(destination), std::cbegin(destination) + count,
									[&](auto &node) noexcept
									{
										return node.Name() == candidate.Name() && node.Classes() == candidate.Classes();
									}); iter == std::cbegin(destination) + count)
									//Candidate is approved
									destination.push_back(candidate);
							}
							break;
						}
						else
							[[fallthrough]];
					}

					case AppendCondition::NoDuplicateNames:
					{
						for (auto &candidate : source)
						{
							if (auto iter = std::find_if(std::cbegin(destination), std::cbegin(destination) + count,
								[&](auto &node) noexcept
								{
									return node.Name() == candidate.Name();
								}); iter == std::cbegin(destination) + count)
								//Candidate is approved
								destination.push_back(candidate);
						}
						break;
					}
				}
			}


			/*
				Printing
			*/

			std::string print(const ObjectNodes &objects, PrintOptions print_options);


			/*
				Searching
			*/

			void breadth_first_search_impl(search_result &result, size_t off);
			void depth_first_search_post_order_impl(search_result &result, const TreeNode &node);
			void depth_first_search_pre_order_impl(search_result &result, const TreeNode &node);
			void fully_qualified_name_impl(std::string &name, const ObjectNode &parent, const ObjectNode &what_object);
			
			search_result breadth_first_search(const ObjectNodes &objects);
			search_result depth_first_search(const ObjectNodes &objects, DepthFirstTraversal traversal);
			std::string fully_qualified_name(const ObjectNodes &objects, const ObjectNode &what_object);

			void lineage_depth_first_search_impl(lineage_search_result &result, generations &descendants, ObjectNode &object);
			lineage_search_result lineage_depth_first_search(ObjectNodes &objects);
		} //detail


		/*
			Node types
		*/

		///@brief A base class representing a tree node
		struct TreeNode final
		{
			const ObjectNode &Object;
			const ObjectNode *const Parent = nullptr;
			const int Depth = 0;

			///@brief Constructs a new tree node containing the given root object
			TreeNode(const ObjectNode &object) noexcept;

			///@brief Constructs a new tree node containing the given child object, parent object and depth
			TreeNode(const ObjectNode &object, const ObjectNode &parent, int depth) noexcept;
		};

		///@brief A class representing an object node in the tree
		///@details An object node is child of another object node and the parent of property nodes and other object nodes
		class ObjectNode final
		{
			private:

				std::string name_;
				std::string classes_;
				PropertyNodes properties_;
				ObjectNodes objects_;

			public:

				///@brief Constructs a new object node with the given name and properties
				ObjectNode(std::string name, std::string classes, PropertyNodes properties) noexcept;

				///@brief Constructs a new object node with the given name, properties and child objects
				ObjectNode(std::string name, std::string classes, PropertyNodes properties, ObjectNodes objects) noexcept;


				/**
					@name Operators
					@{
				*/

				///@brief Returns true if this object is valid
				[[nodiscard]] inline operator bool() const noexcept
				{
					return !std::empty(name_);
				}

				///@}

				/**
					@name Observers
					@{
				*/

				///@brief Returns the name of this object
				[[nodiscard]] inline auto& Name() const noexcept
				{
					return name_;
				}

				///@brief Returns the classes of this object
				[[nodiscard]] inline auto& Classes() const noexcept
				{
					return classes_;
				}

				///@}

				/**
					@name Appending
					@{
				*/

				///@brief Appends all of the given objects that satisfy the given append condition
				void Append(const ObjectNodes &objects, AppendCondition append_condition = AppendCondition::Unconditionally);

				///@brief Appends all of the given mutable iterable objects that satisfy the given append condition
				void Append(const adaptors::ranges::Iterable<ObjectNodes&> &objects, AppendCondition append_condition = AppendCondition::Unconditionally);

				///@brief Appends all of the given immutable iterable objects that satisfy the given append condition
				void Append(const adaptors::ranges::Iterable<const ObjectNodes&> &objects, AppendCondition append_condition = AppendCondition::Unconditionally);


				///@brief Appends all of the given properties that satisfy the given append condition
				void Append(const PropertyNodes &properties, AppendCondition append_condition = AppendCondition::Unconditionally);

				///@brief Appends all of the given mutable iterable properties that satisfy the given append condition
				void Append(const adaptors::ranges::Iterable<PropertyNodes&> &properties, AppendCondition append_condition = AppendCondition::Unconditionally);

				///@brief Appends all of the given immutable iterable properties that satisfy the given append condition
				void Append(const adaptors::ranges::Iterable<const PropertyNodes&> &properties, AppendCondition append_condition = AppendCondition::Unconditionally);

				///@}

				/**
					@name Finding/searching
					@{
				*/

				///@brief Finds an object (top-level child object) by the given name, and return a mutable reference to it
				///@details Returns InvalidObjectNode if the given object is not found
				[[nodiscard]] ObjectNode& Find(std::string_view name) noexcept;

				///@brief Finds an object (top-level child object) by the given name, and return an immutable reference to it
				///@details Returns InvalidObjectNode if the given object is not found
				[[nodiscard]] const ObjectNode& Find(std::string_view name) const noexcept;


				///@brief Searches for an object (all child objects) by the given name and search strategy and return a mutable reference to it
				///@details Returns InvalidObjectNode if the given object is not found
				[[nodiscard]] ObjectNode& Search(std::string_view name, SearchStrategy strategy = SearchStrategy::BreadthFirst) noexcept;

				///@brief Searches for an object (all child objects) by the given name and search strategy and return an immutable reference to it
				///@details Returns InvalidObjectNode if the given object is not found
				[[nodiscard]] const ObjectNode& Search(std::string_view name, SearchStrategy strategy = SearchStrategy::BreadthFirst) const noexcept;


				///@brief Finds a property by the given name and return a mutable reference to it
				///@details Returns InvalidPropertyNode if the given property is not found
				[[nodiscard]] PropertyNode& Property(std::string_view name) noexcept;

				///@brief Finds a property by the given name and return an immutable reference to it
				///@details Returns InvalidPropertyNode if the given property is not found
				[[nodiscard]] const PropertyNode& Property(std::string_view name) const noexcept;

				///@}

				/**
					@name Ranges
					@{
				*/

				///@brief Returns a mutable range of all top-level child objects in this object
				///@details This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Objects() noexcept
				{
					return adaptors::ranges::Iterable<ObjectNodes&>{objects_};
				}

				///@brief Returns an immutable range of all top-level child objects in this object
				///@details This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Objects() const noexcept
				{
					return adaptors::ranges::Iterable<const ObjectNodes&>{objects_};
				}

				///@brief Returns a mutable range of all properties in this object
				///@details This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Properties() noexcept
				{
					return adaptors::ranges::Iterable<PropertyNodes&>{properties_};
				}

				///@brief Returns an immutable range of all properties in this object
				///@details This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Properties() const noexcept
				{
					return adaptors::ranges::Iterable<const PropertyNodes&>{properties_};
				}


				///@brief Returns a mutable (BFS) range of all child objects in this object
				///@details This can be used directly with a range-based for loop
				[[nodiscard]] inline auto BreadthFirstSearch()
				{
					return adaptors::ranges::Iterable<detail::search_result>{detail::breadth_first_search(objects_)};
				}

				///@brief Returns an immutable (BFS) range of all child objects in this object
				///@details This can be used directly with a range-based for loop
				[[nodiscard]] inline auto BreadthFirstSearch() const
				{
					return adaptors::ranges::Iterable<const detail::search_result>{detail::breadth_first_search(objects_)};
				}

				///@brief Returns a mutable (DFS) range of all child objects in this object
				///@details This can be used directly with a range-based for loop
				[[nodiscard]] inline auto DepthFirstSearch(DepthFirstTraversal traversal = DepthFirstTraversal::PreOrder)
				{
					return adaptors::ranges::Iterable<detail::search_result>{detail::depth_first_search(objects_, traversal)};
				}

				///@brief Returns an immutable (DFS) range of all child objects in this object
				///@details This can be used directly with a range-based for loop
				[[nodiscard]] inline auto DepthFirstSearch(DepthFirstTraversal traversal = DepthFirstTraversal::PreOrder) const
				{
					return adaptors::ranges::Iterable<const detail::search_result>{detail::depth_first_search(objects_, traversal)};
				}

				///@}
		};

		///@brief A class representing a property node in the tree
		///@details A property node is child of an object node and the parent of argument nodes
		class PropertyNode final
		{
			private:

				std::string name_;
				ArgumentNodes arguments_;

			public:

				///@brief Constructs a new property node with the given name and arguments
				PropertyNode(std::string name, ArgumentNodes arguments) noexcept;


				/**
					@name Operators
					@{
				*/

				///@brief Returns true if this property is valid
				[[nodiscard]] inline operator bool() const noexcept
				{
					return !std::empty(name_);
				}

				///@}

				/**
					@name Observers
					@{
				*/

				///@brief Returns the name of the property
				[[nodiscard]] inline auto& Name() const noexcept
				{
					return name_;
				}

				///@}

				/**
					@name Arguments
					@{
				*/

				///@brief Returns a mutable argument at the given argument number
				[[nodiscard]] ArgumentNode& Argument(int number) noexcept;

				///@brief Returns an immutable argument at the given argument number
				[[nodiscard]] const ArgumentNode& Argument(int number) const noexcept;


				///@brief Returns a mutable argument at the given argument number
				[[nodiscard]] inline auto& operator[](int number) noexcept
				{
					return Argument(number);
				}

				///@brief Returns an immutable argument at the given argument number
				[[nodiscard]] inline auto& operator[](int number) const noexcept
				{
					return Argument(number);
				}


				///@brief Returns the number of arguments in this property
				[[nodiscard]] inline auto NumberOfArguments() const noexcept
				{
					return std::ssize(arguments_);
				}

				///@}

				/**
					@name Ranges
					@{
				*/

				///@brief Returns a mutable range of all arguments in this property
				///@details This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Arguments() noexcept
				{
					return adaptors::ranges::Iterable<ArgumentNodes&>{arguments_};
				}

				///@brief Returns an immutable range of all arguments in this property
				///@details This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Arguments() const noexcept
				{
					return adaptors::ranges::Iterable<const ArgumentNodes&>{arguments_};
				}

				///@}
		};

		///@brief A class representing an argument node in the tree
		///@details An argument node is child of a property node and contains a value with an associated unit
		class ArgumentNode final
		{
			private:

				std::optional<ArgumentType> argument_;
				std::string unit_;
			
			public:

				///@brief Constructs a new argument node with the given argument
				ArgumentNode(ArgumentType argument) noexcept;

				///@brief Constructs a new argument node with the given argument and unit
				ArgumentNode(ArgumentType argument, std::string unit) noexcept;

				///@brief Constructs an invalid argument node
				explicit ArgumentNode(std::nullopt_t) noexcept;


				/**
					@name Operators
					@{
				*/

				///@brief Returns true if this argument is valid
				[[nodiscard]] inline operator bool() const noexcept
				{
					return argument_.has_value();
				}

				///@}

				/**
					@name Observers
					@{
				*/

				///@brief Returns the value of the argument with the given argument type
				template <typename T>
				[[nodiscard]] inline auto Get() const noexcept
				{
					auto value = *this ? std::get_if<T>(&*argument_) : nullptr;

					if constexpr (std::is_same_v<T, ScriptType::FloatingPoint>)
					{		
						if (!value)
						{
							//Try to get as integer
							if (auto val = *this ? std::get_if<ScriptType::Integer>(&*argument_) : nullptr; val)
								//Okay, non-narrowing
								return std::make_optional(ScriptType::FloatingPoint{val->As<ScriptType::FloatingPoint::value_type>()});
						}
					}

					return value ? std::make_optional(*value) : std::nullopt;
				}

				///@brief Calls the correct overload for the given overload set, based on the value of the argument
				template <typename T, typename ...Ts>
				inline auto Visit(T &&callable, Ts &&...callables) const noexcept
				{
					assert(*this);
					return std::visit(types::overloaded{std::forward<T>(callable), std::forward<Ts>(callables)...}, *argument_);
				}

				///@brief Returns the unit of this argument
				[[nodiscard]] inline auto& Unit() const noexcept
				{
					return unit_;
				}

				///@}
		};
	} //script_tree


	///@brief A class representing a tree structure with object, property and argument nodes
	///@details The tree and nodes are created during compilation of one or more translation units.
	///A script tree needs to be validated against a validation scheme for it to be considered correct.
	///A script tree can be validated then serialized, to later be deserialized without the need to be validated again
	class ScriptTree final
	{
		private:

			script_tree::ObjectNodes objects_;

		public:

			ScriptTree() = default;

			///@brief Constructs a new script tree with the given objects
			explicit ScriptTree(script_tree::ObjectNodes objects) noexcept;


			/**
				@name Appending
				@{
			*/

			///@brief Appends all of the given objects that satisfy the given append condition
			void Append(const script_tree::ObjectNodes &objects, script_tree::AppendCondition append_condition = script_tree::AppendCondition::Unconditionally);

			///@brief Appends all of the given mutable iterable objects that satisfy the given append condition
			void Append(const adaptors::ranges::Iterable<script_tree::ObjectNodes&> &objects, script_tree::AppendCondition append_condition = script_tree::AppendCondition::Unconditionally);

			///@brief Appends all of the given immutable iterable objects that satisfy the given append condition
			void Append(const adaptors::ranges::Iterable<const script_tree::ObjectNodes&> &objects, script_tree::AppendCondition append_condition = script_tree::AppendCondition::Unconditionally);

			///@}

			/**
				@name Finding/searching
				@{
			*/

			///@brief Finds an object (top-level object) by the given name, and return a mutable reference to it
			///@details Returns InvalidObjectNode if the given object is not found
			[[nodiscard]] script_tree::ObjectNode& Find(std::string_view name) noexcept;

			///@brief Finds an object (top-level object) by the given name, and return an immutable reference to it
			///@details Returns InvalidObjectNode if the given object is not found
			[[nodiscard]] const script_tree::ObjectNode& Find(std::string_view name) const noexcept;


			///@brief Searches for an object (all objects) by the given name and search strategy and return a mutable reference to it
			///@details Returns InvalidObjectNode if the given object is not found
			[[nodiscard]] script_tree::ObjectNode& Search(std::string_view name, script_tree::SearchStrategy strategy = script_tree::SearchStrategy::BreadthFirst) noexcept;

			///@brief Searches for an object (all objects) by the given name and search strategy and return an immutable reference to it
			///@details Returns InvalidObjectNode if the given object is not found
			[[nodiscard]] const script_tree::ObjectNode& Search(std::string_view name, script_tree::SearchStrategy strategy = script_tree::SearchStrategy::BreadthFirst) const noexcept;

			///@}

			/**
				@name Fully qualified names
				@{
			*/

			///@brief Returns the fully qualified name to the given object
			///@details Returns nullopt if the given object is not found in this tree
			[[nodiscard]] std::optional<std::string> GetFullyQualifiedName(const script_tree::ObjectNode &object) const;

			///@brief Returns the fully qualified name to the given property
			///@details Returns nullopt if the given object or property is not found in this tree
			[[nodiscard]] std::optional<std::string> GetFullyQualifiedName(const script_tree::ObjectNode &object, const script_tree::PropertyNode &property) const;

			///@}

			/**
				@name Printing
				@{
			*/

			///@brief Prints out this script tree as an hierarchical list with objects, properties and arguments.
			///@details Choose what to print based on the given print options (optionally)
			[[nodiscard]] std::string Print(script_tree::PrintOptions print_options = script_tree::PrintOptions::Arguments) const;

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all top-level objects in this script tree
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Objects() noexcept
			{
				return adaptors::ranges::Iterable<script_tree::ObjectNodes&>{objects_};
			}

			///@brief Returns an immutable range of all top-level objects in this script tree
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Objects() const noexcept
			{
				return adaptors::ranges::Iterable<const script_tree::ObjectNodes&>{objects_};
			}


			///@brief Returns a mutable (BFS) range of all objects in this script tree
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto BreadthFirstSearch()
			{
				return adaptors::ranges::Iterable<script_tree::detail::search_result>{script_tree::detail::breadth_first_search(objects_)};
			}

			///@brief Returns an immutable (BFS) range of all objects in this script tree
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto BreadthFirstSearch() const
			{
				return adaptors::ranges::Iterable<const script_tree::detail::search_result>{script_tree::detail::breadth_first_search(objects_)};
			}

			///@brief Returns a mutable (DFS) range of all objects in this script tree
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto DepthFirstSearch(script_tree::DepthFirstTraversal traversal = script_tree::DepthFirstTraversal::PreOrder)
			{
				return adaptors::ranges::Iterable<script_tree::detail::search_result>{script_tree::detail::depth_first_search(objects_, traversal)};
			}

			///@brief Returns an immutable (DFS) range of all objects in this script tree
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto DepthFirstSearch(script_tree::DepthFirstTraversal traversal = script_tree::DepthFirstTraversal::PreOrder) const
			{
				return adaptors::ranges::Iterable<const script_tree::detail::search_result>{script_tree::detail::depth_first_search(objects_, traversal)};
			}

			///@}

			/**
				@name Serializing
				@{
			*/

			///@brief Deserializes a given byte array to a script tree
			[[nodiscard]] static std::optional<ScriptTree> Deserialize(std::string_view bytes);

			///@brief Serializes this script tree to a byte array
			[[nodiscard]] std::vector<std::byte> Serialize() const;

			///@}
	};

	namespace script_tree
	{
		/**
			@name Predefined node constants
			For fluent interface design
			@{
		*/

		inline const auto InvalidObjectNode = ObjectNode{"", "", {}};
		inline const auto InvalidPropertyNode = PropertyNode{"", {}};
		inline const auto InvalidArgumentNode = ArgumentNode{std::nullopt};

		///@}
	} //script_tree

} //ion::script

#endif