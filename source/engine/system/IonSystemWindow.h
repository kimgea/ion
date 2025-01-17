/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	system
File:	IonSystemWindow.h
-------------------------------------------
*/

#ifndef ION_SYSTEM_WINDOW_H
#define ION_SYSTEM_WINDOW_H

#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "IonSystemAPI.h"
#include "events/IonListenable.h"
#include "graphics/utilities/IonVector2.h"
#include "system/events/listeners/IonSystemMessageListener.h"

#undef CreateDialog

namespace ion::system
{
	using graphics::utilities::Vector2;

	namespace window
	{
		namespace detail
		{
			using namespace std::string_view_literals;
			constexpr auto class_name = "OpenGL"sv;


			#ifdef ION_WIN32

			LRESULT CALLBACK window_procedure(HWND handle, UINT message, WPARAM w_param, LPARAM l_param) noexcept;

			WNDCLASS make_window_class(std::string_view name, HINSTANCE instance) noexcept;
			PIXELFORMATDESCRIPTOR make_pixel_format_descriptor(int color_depth) noexcept;
			DEVMODE make_device_mode(const Vector2 &full_screen_size, int color_depth) noexcept;


			constexpr auto get_borderless_style() noexcept -> std::pair<DWORD, DWORD>
			{
				return {WS_POPUP,
						WS_EX_APPWINDOW & ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE)};
			}

			constexpr auto get_dialog_style() noexcept -> std::pair<DWORD, DWORD>
			{
				return {WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX),
						WS_EX_APPWINDOW | WS_EX_WINDOWEDGE};
			}

			constexpr auto get_single_border_style() noexcept -> std::pair<DWORD, DWORD>
			{
				return {WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
						WS_EX_APPWINDOW | WS_EX_WINDOWEDGE};
			}

			constexpr auto get_sizeable_border_style() noexcept -> std::pair<DWORD, DWORD>
			{
				return {WS_OVERLAPPEDWINDOW,
						WS_EX_APPWINDOW | WS_EX_WINDOWEDGE};
			}

			RECT get_desktop_rectangle() noexcept;
			RECT get_adjusted_window_rectangle(RECT rectangle, DWORD style, DWORD extended_style) noexcept;
			RECT get_non_client_window_rectangle(DWORD style, DWORD extended_style) noexcept;
			RECT get_window_rectangle(RECT client_rectangle, DWORD style, DWORD extended_style) noexcept;
			RECT get_client_window_rectangle(RECT rectangle, DWORD style, DWORD extended_style) noexcept;
			RECT get_centered_window_rectangle(RECT rectangle) noexcept;

			RECT make_window_rectangle(const Vector2 &size, const std::optional<Vector2> &position, DWORD style, DWORD extended_style) noexcept;


			template <typename T>
			class handle_base
			{
				protected:

					T handle_ = nullptr;

				public:

					handle_base() = default;
					handle_base(const handle_base&) = delete;

					handle_base(handle_base &&rhs) noexcept :
						handle_{std::exchange(rhs.handle_, nullptr)}
					{
						//Empty
					}

					virtual ~handle_base() = default;


					handle_base& operator=(const handle_base&) = delete;

					inline auto& operator=(handle_base &&rhs) noexcept
					{
						if (this != &rhs)
						{
							this->~handle_base();
							handle_ = std::exchange(rhs.handle_, nullptr);
						}

						return *this;
					}


					inline auto operator*() const noexcept
					{
						return handle_;
					}

					inline auto operator->() const noexcept
					{
						return handle_;
					}

					inline operator bool() const noexcept
					{
						return !!handle_;
					}
			};

			struct window_class : handle_base<HINSTANCE>
			{
				window_class() = default;
				window_class(HINSTANCE instance) noexcept;
				window_class(window_class &&rhs) noexcept;
				~window_class();

				window_class& operator=(window_class &&rhs) noexcept;
				void reset(window_class &&rhs) noexcept;
			};

			struct window_handle : handle_base<HWND>
			{
				window_handle() = default;
				window_handle(window_class &win_class,
					std::string_view title, const Vector2 &size, const std::optional<Vector2> &position,
					DWORD style, DWORD extended_style, LPVOID parameter) noexcept;
				window_handle(window_handle &&rhs) noexcept;
				~window_handle();

				window_handle& operator=(window_handle &&rhs) noexcept;
				void reset(window_handle &&rhs) noexcept;
			};

			class device_context : public handle_base<HDC>
			{
				private:

					window_handle *win_handle_ = nullptr;

				public:

					device_context() = default;
					device_context(window_handle &win_handle, int color_depth) noexcept;
					device_context(device_context &&rhs) noexcept;
					~device_context();

					device_context& operator=(device_context &&rhs) noexcept;
					void reset(device_context &&rhs) noexcept;
			};

			struct rendering_context : handle_base<HGLRC>
			{
				rendering_context() = default;
				rendering_context(device_context &dev_context) noexcept;
				rendering_context(rendering_context &&rhs) noexcept;
				~rendering_context();

				rendering_context& operator=(rendering_context &&rhs) noexcept;
				void reset(rendering_context &&rhs) noexcept;
			};

			struct full_screen : handle_base<HWND>
			{
				private:

					Vector2 size_;
					Vector2 position_;
					DWORD style_ = {};
					DWORD extended_style_ = {};

				public:

					full_screen() = default;
					full_screen(window_handle &win_handle, device_context &dev_context,
						const std::optional<Vector2> &full_screen_size) noexcept;
					full_screen(full_screen &&rhs) noexcept;
					~full_screen();

					full_screen& operator=(full_screen &&rhs) noexcept;
					void reset(full_screen &&rhs) noexcept;
			};


			bool set_title(std::string_view title, HWND handle) noexcept;
			bool set_size(const Vector2 &size, HWND handle) noexcept;
			bool set_client_size(const Vector2 &size, HWND handle) noexcept;	
			bool set_position(const Vector2 &position, HWND handle) noexcept;
			bool set_client_position(const Vector2 &position, HWND handle) noexcept;
			bool set_extents_and_border_style(const Vector2 &size, const Vector2 &position,
				DWORD style, DWORD extended_style, HWND handle) noexcept;

			bool set_border_style(DWORD style, DWORD extended_style, HWND handle) noexcept;
			bool set_borderless_style(HWND handle) noexcept;
			bool set_dialog_border_style(HWND handle) noexcept;
			bool set_single_border_style(HWND handle) noexcept;
			bool set_sizeable_border_style(HWND handle) noexcept;

			bool show_cursor() noexcept;
			bool hide_cursor() noexcept;

			bool show_window(int cmd_show, HWND handle) noexcept;
			bool hide_window(HWND handle) noexcept;
			bool center_window(HWND handle) noexcept;
			bool focus_window(HWND handle) noexcept;

			DWORD get_window_style(HWND handle) noexcept;
			DWORD get_extended_window_style(HWND handle) noexcept;
			RECT get_window_rectangle(HWND handle) noexcept;
			RECT get_client_window_rectangle(HWND handle) noexcept;
			RECT get_centered_window_rectangle(HWND handle) noexcept;

			Vector2 get_size(HWND handle) noexcept;
			Vector2 get_client_size(HWND handle) noexcept;
			Vector2 get_desktop_size() noexcept;
			Vector2 get_position(HWND handle) noexcept;
			Vector2 get_client_position(HWND handle) noexcept;

			bool is_active(HWND handle) noexcept;
			bool is_centered(HWND handle) noexcept;

			#endif
		} //detail
	} //window


	///@brief A class representing a system specific window (window for messages, inputs and OpenGL rendering)
	///@details Functions, classes and class members may need different implementation based on the underlying OS.
	///System specific code should have its own define directive
	class Window :
		public ion::events::Listenable<events::listeners::MessageListener>
	{
		private:

			#ifdef ION_WIN32
			window::detail::window_class class_;
			window::detail::window_handle handle_;
			window::detail::device_context dev_context_;
			window::detail::rendering_context gl_context_;
			window::detail::full_screen full_screen_;
			#endif


			#ifdef ION_WIN32
			///@brief Processes a message that is sent to the window
			bool ProcessMessage(HWND handle, UINT message, WPARAM w_param, LPARAM l_param) noexcept;
			#endif


			/**
				@name Notifying
				@{
			*/

			#ifdef ION_WIN32
			bool NotifyMessageReceived(HWND handle, UINT message, WPARAM w_param, LPARAM l_param) noexcept;
			#endif

			///@}

			/**
				@name Creating/destroying
				@{
			*/

			bool DoCreate(std::string_view title, const Vector2 &size,
				const std::optional<Vector2> &position, int color_depth,
				DWORD style, DWORD extended_style) noexcept;

			void DoDestroy() noexcept;

			///@}

			/**
				@name Window events
				@{
			*/

			///@brief Called right after a window has been created/opened
			virtual void Opened() noexcept;

			///@brief Called right after a window has been closed/destroyed
			virtual void Closed() noexcept;


			///@brief Called right after a window has been activated
			virtual void Activated() noexcept;

			///@brief Called right after a window has been deactivated
			virtual void Deactivated() noexcept;


			///@brief Called right after a window has been maximized
			virtual void Maximized() noexcept;

			///@brief Called right after a window has been minimized
			virtual void Minimized() noexcept;

			///@brief Called right after a window has been restored
			virtual void Restored() noexcept;


			///@brief Called right after a window has been moved, with the new position
			virtual void Moved(const Vector2 &position) noexcept;

			///@brief Called right after a window has been resized, with the new size
			virtual void Resized(const Vector2 &size) noexcept;

			///@brief Called right after a window display mode has been changed
			virtual void DisplayModeChanged() noexcept;


			///@brief Called to retrieve the full screen size, if any
			virtual std::optional<Vector2> GetFullScreenSize() const noexcept;

			///@brief Called to retrieve the min and max size constraints, if any
			virtual std::pair<std::optional<Vector2>, std::optional<Vector2>> GetSizeConstraints() const noexcept;

			///@}

		public:

			///@brief Default constructor
			Window() = default;

			///@brief Deleted copy constructor
			Window(const Window&) = delete;

			///@brief Default move constructor
			Window(Window&&) = default;

			///@brief Default virtual destructor
			virtual ~Window() = default;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			Window& operator=(const Window&) = delete;

			///@brief Default move assignment
			Window& operator=(Window&&) = default;

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the window title to the given title
			inline void Title(std::string_view title) noexcept
			{
				#ifdef ION_WIN32
				if (handle_)
					window::detail::set_title(title, *handle_);
				#endif
			}

			///@brief Sets the window inner size to the given size
			inline void InnerSize(const Vector2 &size) noexcept
			{
				#ifdef ION_WIN32
				if (handle_)
					window::detail::set_client_size(size, *handle_);
				#endif
			}

			///@brief Sets the window outer size to the given size
			inline void OuterSize(const Vector2 &size) noexcept
			{
				#ifdef ION_WIN32
				if (handle_)
					window::detail::set_size(size, *handle_);
				#endif
			}

			///@brief Sets the window inner position to the given position
			inline void InnerPosition(const Vector2 &position) noexcept
			{
				#ifdef ION_WIN32
				if (handle_)
					window::detail::set_client_position(position, *handle_);
				#endif
			}

			///@brief Sets the window outer position to the given position
			inline void OuterPosition(const Vector2 &position) noexcept
			{
				#ifdef ION_WIN32
				if (handle_)
					window::detail::set_position(position, *handle_);
				#endif
			}


			///@brief Enters full screen with the given size. If no size is given, it uses the desktop size
			///@details Returns true if the window successfully entered full screen
			inline auto EnterFullScreen(const std::optional<Vector2> &full_screen_size) noexcept
			{
				#ifdef ION_WIN32
				if (handle_ && !full_screen_)
				{
					DisplayModeChanged();
					full_screen_.reset({handle_, dev_context_, full_screen_size});
				}

				return !!full_screen_;
				#else
				return false;
				#endif
			}

			///@brief Exits out of full screen
			///@details Returns true if the window successfully exited out of full screen
			inline auto ExitFullScreen() noexcept
			{
				#ifdef ION_WIN32
				if (handle_ && full_screen_)
				{		
					full_screen_.reset({});
					DisplayModeChanged();
				}

				return !full_screen_;
				#else
				return false;
				#endif
			}


			///@brief Switches to a borderless window style
			inline void BorderlessStyle() noexcept
			{
				#ifdef ION_WIN32
				if (handle_)
					window::detail::set_borderless_style(*handle_);
				#endif
			}

			///@brief Switches to a dialog window style
			inline void DialogStyle() noexcept
			{
				#ifdef ION_WIN32
				if (handle_)
					window::detail::set_dialog_border_style(*handle_);
				#endif
			}

			///@brief Switches to a single border window style
			inline void SingleBorderStyle() noexcept
			{
				#ifdef ION_WIN32
				if (handle_)
					window::detail::set_single_border_style(*handle_);
				#endif
			}

			///@brief Switches to a sizeable border window style
			inline void SizeableBorderStyle() noexcept
			{
				#ifdef ION_WIN32
				if (handle_)
					window::detail::set_sizeable_border_style(*handle_);
				#endif
			}


			///@brief Shows the window cursor
			inline void ShowCursor() noexcept
			{
				#ifdef ION_WIN32
				window::detail::show_cursor();
				#endif
			}

			///@brief Hides the window cursor
			inline void HideCursor() noexcept
			{
				#ifdef ION_WIN32
				window::detail::hide_cursor();
				#endif
			}


			///@brief Focuses the window by bringing it to the front
			inline void Focus() noexcept
			{
				#ifdef ION_WIN32
				if (handle_)
					window::detail::focus_window(*handle_);
				#endif
			}

			///@brief Sets the window position to the center of the main desktop
			inline void Center() noexcept
			{
				#ifdef ION_WIN32
				if (handle_)
					window::detail::center_window(*handle_);
				#endif
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the inner (client/screen) size of the window
			///@details Returns nullopt if no window has been created
			[[nodiscard]] inline auto InnerSize() const noexcept
			{
				#ifdef ION_WIN32
				if (handle_)
					return std::make_optional(window::detail::get_client_size(*handle_));
				#endif
				
				return std::optional<Vector2>{};
			}

			///@brief Returns the outer size of the window
			///@details Returns nullopt if no window has been created
			[[nodiscard]] inline auto OuterSize() const noexcept
			{
				#ifdef ION_WIN32
				if (handle_)
					return std::make_optional(window::detail::get_size(*handle_));
				#endif
				
				return std::optional<Vector2>{};
			}

			///@brief Returns the desktop size of the system
			///@details Returns nullopt if system does not have a desktop
			[[nodiscard]] inline auto DesktopSize() const noexcept
			{
				#ifdef ION_WIN32
				return std::make_optional(window::detail::get_desktop_size());
				#else			
				return std::optional<Vector2>{};
				#endif
			}

			///@brief Returns the inner (client/screen) position of the window
			///@details Returns nullopt if no window has been created
			[[nodiscard]] inline auto InnerPosition() const noexcept
			{
				#ifdef ION_WIN32
				if (handle_)
					return std::make_optional(window::detail::get_client_position(*handle_));
				#endif
				
				return std::optional<Vector2>{};
			}

			///@brief Returns the outer position of the window
			///@details Returns nullopt if no window has been created
			[[nodiscard]] inline auto OuterPosition() const noexcept
			{
				#ifdef ION_WIN32
				if (handle_)
					return std::make_optional(window::detail::get_position(*handle_));
				#endif
				
				return std::optional<Vector2>{};
			}


			///@brief Returns true if the window is created
			[[nodiscard]] inline auto Created() const noexcept
			{
				#ifdef ION_WIN32
				return !!handle_;
				#else
				return false;
				#endif
			}

			///@brief Returns true if the window is active (in focus)
			[[nodiscard]] inline auto IsActive() const noexcept
			{
				#ifdef ION_WIN32
				return handle_ && window::detail::is_active(*handle_);
				#else
				return false;
				#endif
			}

			///@brief Returns true if the window is centered
			[[nodiscard]] inline auto IsCentered() const noexcept
			{
				#ifdef ION_WIN32
				return handle_ && window::detail::is_centered(*handle_);
				#else
				return false;
				#endif
			}

			///@brief Returns true if the window is in full screen mode
			[[nodiscard]] inline auto InFullScreen() const noexcept
			{
				#ifdef ION_WIN32
				return !!full_screen_;
				#else
				return false;
				#endif
			}


			#ifdef ION_WIN32
			///@brief Returns the window handle
			///@details Returns nullptr if no window has been created
			[[nodiscard]] inline auto Handle() const noexcept
			{
				return *handle_;
			}
			#else
			[[nodiscard]] inline auto Handle() const noexcept -> void*
			{
				return nullptr;
			}
			#endif

			///@}

			/**
				@name Buffers
				@{
			*/

			///@brief Exchanges the front and back buffers
			void SwapBuffers() const noexcept;

			///@}

			/**
				@name Create/destroy
				@{
			*/

			///@brief Creates a borderless system window
			bool CreateBorderless(std::string_view title, const Vector2 &size, const std::optional<Vector2> &position, int color_depth = 32) noexcept;

			///@brief Creates a dialog system window
			bool CreateDialog(std::string_view title, const Vector2 &size, const std::optional<Vector2> &position, int color_depth = 32) noexcept;

			///@brief Creates a single border system window
			bool CreateNonResizable(std::string_view title, const Vector2 &size, const std::optional<Vector2> &position, int color_depth = 32) noexcept;

			///@brief Creates a sizeable border system window
			bool CreateResizable(std::string_view title, const Vector2 &size, const std::optional<Vector2> &position, int color_depth = 32) noexcept;


			///@brief Destroyes a system window (if existing)
			void Destroy() noexcept;

			///@}

			/**
				@name Show/hide
				@{
			*/

			///@brief Shows and focus the system window
			///@details Makes the window visible
			bool Show() noexcept;

			///@brief Hides the system window
			///@details Makes the window not visible
			bool Hide() noexcept;

			///@}

			/**
				@name Messages
				@{
			*/

			///@brief Process all messages in the message queue
			///@details Returns false if a quit message has been received
			bool ProcessMessages() noexcept;

			///@brief Process the next message in the message queue
			///@details Returns false if no message remains in queue
			bool ProcessNextMessage(bool &quit) noexcept;

			#ifdef ION_WIN32
			///@brief An application-defined function that processes each dispatched message from the 
			LRESULT CALLBACK Procedure(HWND handle, UINT message, WPARAM w_param, LPARAM l_param) noexcept;
			#endif

			///@}
	};

} //ion::system

#endif