// imgui-notify by patrickcjk
// https://github.com/patrickcjk/imgui-notify

#ifndef IMGUI_NOTIFY
#define IMGUI_NOTIFY

#pragma once
#include <vector>
#include "font_awesome_5.h"
#include "fa-solid-900.h"

#define NOTIFY_MAX_MSG_LENGTH		255			// Max message content length
#define NOTIFY_PADDING_X			20.f		// Bottom-left X padding
#define NOTIFY_PADDING_Y			20.f		// Bottom-left Y padding
#define NOTIFY_PADDING_MESSAGE_Y	10.f		// Padding Y between each message
#define NOTIFY_FADE_IN_OUT_TIME		150			// Find in and out duration
#define NOTIFY_DEFAULT_DISMISS		3000		// Auto dismiss after X ms
#define NOTIFY_OPACITY				1.0f		// 0-1 Toast opacity

#define NOTIFY_COLOR_ERROR			{ 255, 0, 0, 255 }		// Error
#define NOTIFY_COLOR_WARNING		{ 255, 255, 0, 255 }	// Yellow
#define NOTIFY_COLOR_SUCCESS		{ 0, 255, 0, 255 }		// Green
#define NOTIFY_COLOR_INFO			{ 255, 255, 255, 255 }	// White
#define NOTIFY_COLOR_DEFAULT		{ 255, 255, 255, 255 }	// White

#define NOTIFY_TOAST_FLAGS			ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing

enum class toast_type
{
	toast_type_none,
	toast_type_success,
	toast_type_warning,
	toast_type_error,
	toast_type_info,
	toast_type_COUNT
};
extern int horizontal;
extern int vertical;
enum class toast_phase
{
	toast_phase_fade_in,
	toast_phase_wait,
	toast_phase_fade_out,
	toast_phase_expired,
	toast_phase_COUNT
};

class toast
{
public:
	toast_type		type = toast_type::toast_type_none;
	char			content[NOTIFY_MAX_MSG_LENGTH];
	int				dismiss_time = NOTIFY_DEFAULT_DISMISS;
	uint64_t		creation_time;

	toast(toast_type type, int dismiss_time = NOTIFY_DEFAULT_DISMISS)
	{
		this->type = type;
		this->dismiss_time = dismiss_time;
		this->creation_time = GetTickCount64();
	}

	toast(toast_type type, const char* format, ...) : toast(type)
	{
		va_list args;
		va_start(args, format);
		vsnprintf(this->content, sizeof(this->content), format, args);
		va_end(args);
	}

	toast(toast_type type, int dismiss_time, const char* format, ...) : toast(type, dismiss_time)
	{
		va_list args;
		va_start(args, format);
		vsnprintf(this->content, sizeof(this->content), format, args);
		va_end(args);
	}

	auto get_color() -> ImVec4
	{
		switch (type)
		{
		case toast_type::toast_type_success:
			return NOTIFY_COLOR_SUCCESS;
		case toast_type::toast_type_warning:
			return NOTIFY_COLOR_WARNING;
		case toast_type::toast_type_error:
			return NOTIFY_COLOR_ERROR;
		case toast_type::toast_type_info:
			return NOTIFY_COLOR_INFO;
		default:
			return NOTIFY_COLOR_DEFAULT;
		}
	}

	auto get_icon() -> const char*
	{
		switch (type)
		{
		case toast_type::toast_type_none:
			return NULL;
		case toast_type::toast_type_success:
			return ICON_FA_CHECK_CIRCLE;
		case toast_type::toast_type_warning:
			return ICON_FA_EXCLAMATION_TRIANGLE;
		case toast_type::toast_type_error:
			return ICON_FA_TIMES_CIRCLE;
		case toast_type::toast_type_info:
		default:
			return ICON_FA_INFO_CIRCLE;
		}
	}

	auto get_elapsed_time()
	{
		return GetTickCount64() - this->creation_time;
	}

	auto get_phase() -> toast_phase
	{
		const auto elapsed = get_elapsed_time();

		if (elapsed > NOTIFY_FADE_IN_OUT_TIME + this->dismiss_time + NOTIFY_FADE_IN_OUT_TIME)
		{
			return toast_phase::toast_phase_expired;
		}
		else if (elapsed > NOTIFY_FADE_IN_OUT_TIME + this->dismiss_time)
		{
			return toast_phase::toast_phase_fade_out;
		}
		else if (elapsed > NOTIFY_FADE_IN_OUT_TIME)
		{
			return toast_phase::toast_phase_wait;
		}
		else
		{
			return toast_phase::toast_phase_fade_in;
		}
	}

	auto get_fade_percent() -> float
	{
		auto phase = get_phase();

		const auto elapsed = get_elapsed_time();

		if (phase == toast_phase::toast_phase_fade_in)
		{
			return (float)elapsed / (float)NOTIFY_FADE_IN_OUT_TIME;
		}
		else if (phase == toast_phase::toast_phase_fade_out)
		{
			return 1.f - (((float)elapsed - (float)NOTIFY_FADE_IN_OUT_TIME - (float)this->dismiss_time) / (float)NOTIFY_FADE_IN_OUT_TIME);
		}

		return 1.f;
	}
};

namespace notify
{
	extern std::vector<toast> toast_list;
	extern int NotfiMenuSize;

	/// <summary>
	/// Insert a new toast in the list
	/// </summary>
	inline void insert(const toast& msg)
	{
		toast_list.push_back(msg);
	}

	/// <summary>
	/// Remove a toast from the list by its index
	/// </summary>
	/// <param name="index">index of the toast to remove</param>
	inline void remove(int index)
	{
		toast_list.erase(toast_list.begin() + index);
	}
	/// <summary>
	/// Render toasts, call at the end of your rendering!
	/// </summary>
	inline void render()
	{
		float height = 0.f;

		for (auto i = 0; i < toast_list.size(); i++)
		{
			auto* current_toast = &toast_list[i];

			// Remove toast if expired
			if (current_toast->get_phase() == toast_phase::toast_phase_expired)
			{
				remove(i);
				continue;
			}

			// Generate new unique name for this toast
			char window_name[50];
			sprintf_s(window_name, "##TOAST%d", i);

			// Get opacity based on the current phase
			auto opacity = NOTIFY_OPACITY * current_toast->get_fade_percent();
			ImVec4 TextPopWindowColour = ImColor(0, 0, 0, 255);
			ImVec4 TextPopWindowBor = ImColor(255, 255, 255, 255);

			// Window rendering
			auto text_color = current_toast->get_color(); text_color.w = opacity;
			ImGui::PushStyleColor(ImGuiCol_Text, text_color);
			ImGui::PushStyleColor(ImGuiCol_WindowBg, TextPopWindowColour);
			ImGui::PushStyleColor(ImGuiCol_Border, TextPopWindowBor);
			ImGui::SetNextWindowBgAlpha(opacity);

			// Get the actual window size
			ImVec2 window_size = ImGui::GetIO().DisplaySize;

			// Calculate the position and size of the toast window
			ImVec2 window_pos = ImVec2(window_size.x - NOTIFY_PADDING_X, window_size.y - NOTIFY_PADDING_Y - height);
			ImVec2 window_size_final = ImVec2(250, 50 * NotfiMenuSize);

			ImGui::SetNextWindowSize(window_size_final);
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2(1.0f, 1.0f));

			if (ImGui::Begin(window_name, NULL, NOTIFY_TOAST_FLAGS))
			{
				char buffer[NOTIFY_MAX_MSG_LENGTH];
				if (current_toast->type != toast_type::toast_type_none)
				{
					sprintf_s(buffer, "%s  %s", current_toast->get_icon(), current_toast->content);
				}
				else
				{
					strcpy_s(buffer, current_toast->content);
				}

				float win_width = ImGui::GetWindowSize().x;
				float text_width = ImGui::CalcTextSize(buffer).x;
				float text_indentation = (win_width - text_width) * 0.5f;
				float min_indentation = 20.0f;

				if (text_indentation <= min_indentation) {
					text_indentation = min_indentation;
					NotfiMenuSize++;
				}

				ImGui::SameLine(text_indentation);
				ImGui::PushTextWrapPos(win_width - text_indentation);
				ImGui::TextWrapped(buffer);
				ImGui::PopTextWrapPos();
			}

			// Save height for next toasts
			height += ImGui::GetWindowHeight() + NOTIFY_PADDING_MESSAGE_Y;

			// End
			ImGui::End();
			ImGui::PopStyleColor(3);
			NotfiMenuSize = 0;
		}
	}

	/// <summary>
	/// Adds font-awesome font, must be called ONCE on initialization
	/// <param name="FontDataOwnedByAtlas">Fonts are loaded from read-only memory, should be set to false!</param>
	/// </summary>
	inline void init(bool FontDataOwnedByAtlas = false)
	{
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		
		ImFontConfig icons_config; 
		icons_config.MergeMode = true; 
		icons_config.PixelSnapH = true;
		icons_config.FontDataOwnedByAtlas = FontDataOwnedByAtlas;
		ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)fa_solid_900, sizeof(fa_solid_900), 20.f, &icons_config, icons_ranges);
	}
}

#endif
