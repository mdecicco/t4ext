#include <bindings/Bindings.h>
#include <script/IScriptAPI.hpp>

#include <Client.h>

#include <utils/Math.hpp>
#include <imgui.h>

struct ImDrawListSharedData {};
struct ImFontBuilderIO {};
struct ImGuiContext {};

namespace t4ext {
    void imguiSizeCallback(ImGuiSizeCallbackData* data) {
        Callback<void, const ImVec2&, const ImVec2&>* cb = (Callback<void, const ImVec2&, const ImVec2&>*)data->UserData;
        cb->call(data->CurrentSize, data->DesiredSize);
    }

    void BindImGuiEnums(IScriptAPI* api) {
        DataType* tp_ImGuiKey = api->bind<ImGuiKey>("Key");
        DataType* tp_ImGuiMouseSource = api->bind<ImGuiMouseSource>("MouseSource");
        DataType* tp_ImGuiCol = api->bind<ImGuiCol_>("Color");
        DataType* tp_ImGuiCond = api->bind<ImGuiCond_>("Condition");
        DataType* tp_ImGuiDataType = api->bind<ImGuiDataType_>("DataType");
        DataType* tp_ImGuiDir = api->bind<ImGuiDir_>("Direction");
        DataType* tp_ImGuiMouseButton = api->bind<ImGuiMouseButton_>("MouseButton");
        DataType* tp_ImGuiMouseCursor = api->bind<ImGuiMouseCursor_>("MouseCursor");
        DataType* tp_ImGuiSortDirection = api->bind<ImGuiSortDirection_>("SortDirection");
        DataType* tp_ImGuiStyleVar = api->bind<ImGuiStyleVar_>("StyleVar");
        DataType* tp_ImGuiTableBgTarget = api->bind<ImGuiTableBgTarget_>("TableBgTarget");
        DataType* tp_ImDrawFlags = api->bind<ImDrawFlags_>("DrawFlags");
        DataType* tp_ImDrawListFlags = api->bind<ImDrawListFlags_>("DrawListFlags");
        DataType* tp_ImFontAtlasFlags = api->bind<ImFontAtlasFlags_>("FontAtlasFlags");
        DataType* tp_ImGuiBackendFlags = api->bind<ImGuiBackendFlags_>("BackendFlags");
        DataType* tp_ImGuiButtonFlags = api->bind<ImGuiButtonFlags_>("ButtonFlags");
        DataType* tp_ImGuiChildFlags = api->bind<ImGuiChildFlags_>("ChildFlags");
        DataType* tp_ImGuiColorEditFlags = api->bind<ImGuiColorEditFlags_>("ColorEditFlags");
        DataType* tp_ImGuiConfigFlags = api->bind<ImGuiConfigFlags_>("ConfigFlags");
        DataType* tp_ImGuiComboFlags = api->bind<ImGuiComboFlags_>("ComboFlags");
        DataType* tp_ImGuiDragDropFlags = api->bind<ImGuiDragDropFlags_>("DragDropFlags");
        DataType* tp_ImGuiFocusedFlags = api->bind<ImGuiFocusedFlags_>("FocusedFlags");
        DataType* tp_ImGuiHoveredFlags = api->bind<ImGuiHoveredFlags_>("HoveredFlags");
        DataType* tp_ImGuiInputTextFlags = api->bind<ImGuiInputTextFlags_>("InputTextFlags");
        DataType* tp_ImGuiKeyChord = api->bind<ImGuiKeyChord>("KeyChord");
        DataType* tp_ImGuiPopupFlags = api->bind<ImGuiPopupFlags_>("PopupFlags");
        DataType* tp_ImGuiSelectableFlags = api->bind<ImGuiSelectableFlags_>("SelectableFlags");
        DataType* tp_ImGuiSliderFlags = api->bind<ImGuiSliderFlags_>("SliderFlags");
        DataType* tp_ImGuiTabBarFlags = api->bind<ImGuiTabBarFlags_>("TabBarFlags");
        DataType* tp_ImGuiTabItemFlags = api->bind<ImGuiTabItemFlags_>("TabItemFlags");
        DataType* tp_ImGuiTableFlags = api->bind<ImGuiTableFlags_>("TableFlags");
        DataType* tp_ImGuiTableColumnFlags = api->bind<ImGuiTableColumnFlags_>("TableColumnFlags");
        DataType* tp_ImGuiTableRowFlags = api->bind<ImGuiTableRowFlags_>("TableRowFlags");
        DataType* tp_ImGuiTreeNodeFlags = api->bind<ImGuiTreeNodeFlags_>("TreeNodeFlags");
        DataType* tp_ImGuiViewportFlags = api->bind<ImGuiViewportFlags_>("ViewportFlags");
        DataType* tp_ImGuiWindowFlags = api->bind<ImGuiWindowFlags_>("WindowFlags");
        DataType* tp_ImGuiNavInput = api->bind<ImGuiNavInput>("NavInput");

        tp_ImGuiWindowFlags->bindEnumValue("None", ImGuiWindowFlags_None);
        tp_ImGuiWindowFlags->bindEnumValue("NoTitleBar", ImGuiWindowFlags_NoTitleBar);
        tp_ImGuiWindowFlags->bindEnumValue("NoResize", ImGuiWindowFlags_NoResize);
        tp_ImGuiWindowFlags->bindEnumValue("NoMove", ImGuiWindowFlags_NoMove);
        tp_ImGuiWindowFlags->bindEnumValue("NoScrollbar", ImGuiWindowFlags_NoScrollbar);
        tp_ImGuiWindowFlags->bindEnumValue("NoScrollWithMouse", ImGuiWindowFlags_NoScrollWithMouse);
        tp_ImGuiWindowFlags->bindEnumValue("NoCollapse", ImGuiWindowFlags_NoCollapse);
        tp_ImGuiWindowFlags->bindEnumValue("AlwaysAutoResize", ImGuiWindowFlags_AlwaysAutoResize);
        tp_ImGuiWindowFlags->bindEnumValue("NoBackground", ImGuiWindowFlags_NoBackground);
        tp_ImGuiWindowFlags->bindEnumValue("NoSavedSettings", ImGuiWindowFlags_NoSavedSettings);
        tp_ImGuiWindowFlags->bindEnumValue("NoMouseInputs", ImGuiWindowFlags_NoMouseInputs);
        tp_ImGuiWindowFlags->bindEnumValue("MenuBar", ImGuiWindowFlags_MenuBar);
        tp_ImGuiWindowFlags->bindEnumValue("HorizontalScrollbar", ImGuiWindowFlags_HorizontalScrollbar);
        tp_ImGuiWindowFlags->bindEnumValue("NoFocusOnAppearing", ImGuiWindowFlags_NoFocusOnAppearing);
        tp_ImGuiWindowFlags->bindEnumValue("NoBringToFrontOnFocus", ImGuiWindowFlags_NoBringToFrontOnFocus);
        tp_ImGuiWindowFlags->bindEnumValue("AlwaysVerticalScrollbar", ImGuiWindowFlags_AlwaysVerticalScrollbar);
        tp_ImGuiWindowFlags->bindEnumValue("AlwaysHorizontalScrollbar", ImGuiWindowFlags_AlwaysHorizontalScrollbar);
        tp_ImGuiWindowFlags->bindEnumValue("NoNavInputs", ImGuiWindowFlags_NoNavInputs);
        tp_ImGuiWindowFlags->bindEnumValue("NoNavFocus", ImGuiWindowFlags_NoNavFocus);
        tp_ImGuiWindowFlags->bindEnumValue("UnsavedDocument", ImGuiWindowFlags_UnsavedDocument);
        tp_ImGuiWindowFlags->bindEnumValue("NoNav", ImGuiWindowFlags_NoNav);
        tp_ImGuiWindowFlags->bindEnumValue("NoDecoration", ImGuiWindowFlags_NoDecoration);
        tp_ImGuiWindowFlags->bindEnumValue("NoInputs", ImGuiWindowFlags_NoInputs);
        tp_ImGuiWindowFlags->bindEnumValue("NavFlattened", ImGuiWindowFlags_NavFlattened);
        tp_ImGuiWindowFlags->bindEnumValue("ChildWindow", ImGuiWindowFlags_ChildWindow);
        tp_ImGuiWindowFlags->bindEnumValue("Tooltip", ImGuiWindowFlags_Tooltip);
        tp_ImGuiWindowFlags->bindEnumValue("Popup", ImGuiWindowFlags_Popup);
        tp_ImGuiWindowFlags->bindEnumValue("Modal", ImGuiWindowFlags_Modal);
        tp_ImGuiWindowFlags->bindEnumValue("ChildMenu", ImGuiWindowFlags_ChildMenu);
        tp_ImGuiWindowFlags->bindEnumValue("AlwaysUseWindowPadding", ImGuiWindowFlags_AlwaysUseWindowPadding);

        tp_ImGuiChildFlags->bindEnumValue("None", ImGuiChildFlags_None);
        tp_ImGuiChildFlags->bindEnumValue("Border", ImGuiChildFlags_Border);
        tp_ImGuiChildFlags->bindEnumValue("AlwaysUseWindowPadding", ImGuiChildFlags_AlwaysUseWindowPadding);
        tp_ImGuiChildFlags->bindEnumValue("ResizeX", ImGuiChildFlags_ResizeX);
        tp_ImGuiChildFlags->bindEnumValue("ResizeY", ImGuiChildFlags_ResizeY);
        tp_ImGuiChildFlags->bindEnumValue("AutoResizeX", ImGuiChildFlags_AutoResizeX);
        tp_ImGuiChildFlags->bindEnumValue("AutoResizeY", ImGuiChildFlags_AutoResizeY);
        tp_ImGuiChildFlags->bindEnumValue("AlwaysAutoResize", ImGuiChildFlags_AlwaysAutoResize);
        tp_ImGuiChildFlags->bindEnumValue("FrameStyle", ImGuiChildFlags_FrameStyle);

        tp_ImGuiInputTextFlags->bindEnumValue("None", ImGuiInputTextFlags_None);
        tp_ImGuiInputTextFlags->bindEnumValue("CharsDecimal", ImGuiInputTextFlags_CharsDecimal);
        tp_ImGuiInputTextFlags->bindEnumValue("CharsHexadecimal", ImGuiInputTextFlags_CharsHexadecimal);
        tp_ImGuiInputTextFlags->bindEnumValue("CharsUppercase", ImGuiInputTextFlags_CharsUppercase);
        tp_ImGuiInputTextFlags->bindEnumValue("CharsNoBlank", ImGuiInputTextFlags_CharsNoBlank);
        tp_ImGuiInputTextFlags->bindEnumValue("AutoSelectAll", ImGuiInputTextFlags_AutoSelectAll);
        tp_ImGuiInputTextFlags->bindEnumValue("EnterReturnsTrue", ImGuiInputTextFlags_EnterReturnsTrue);
        tp_ImGuiInputTextFlags->bindEnumValue("CallbackCompletion", ImGuiInputTextFlags_CallbackCompletion);
        tp_ImGuiInputTextFlags->bindEnumValue("CallbackHistory", ImGuiInputTextFlags_CallbackHistory);
        tp_ImGuiInputTextFlags->bindEnumValue("CallbackAlways", ImGuiInputTextFlags_CallbackAlways);
        tp_ImGuiInputTextFlags->bindEnumValue("CallbackCharFilter", ImGuiInputTextFlags_CallbackCharFilter);
        tp_ImGuiInputTextFlags->bindEnumValue("AllowTabInput", ImGuiInputTextFlags_AllowTabInput);
        tp_ImGuiInputTextFlags->bindEnumValue("CtrlEnterForNewLine", ImGuiInputTextFlags_CtrlEnterForNewLine);
        tp_ImGuiInputTextFlags->bindEnumValue("NoHorizontalScroll", ImGuiInputTextFlags_NoHorizontalScroll);
        tp_ImGuiInputTextFlags->bindEnumValue("AlwaysOverwrite", ImGuiInputTextFlags_AlwaysOverwrite);
        tp_ImGuiInputTextFlags->bindEnumValue("ReadOnly", ImGuiInputTextFlags_ReadOnly);
        tp_ImGuiInputTextFlags->bindEnumValue("Password", ImGuiInputTextFlags_Password);
        tp_ImGuiInputTextFlags->bindEnumValue("NoUndoRedo", ImGuiInputTextFlags_NoUndoRedo);
        tp_ImGuiInputTextFlags->bindEnumValue("CharsScientific", ImGuiInputTextFlags_CharsScientific);
        tp_ImGuiInputTextFlags->bindEnumValue("CallbackResize", ImGuiInputTextFlags_CallbackResize);
        tp_ImGuiInputTextFlags->bindEnumValue("CallbackEdit", ImGuiInputTextFlags_CallbackEdit);
        tp_ImGuiInputTextFlags->bindEnumValue("EscapeClearsAll", ImGuiInputTextFlags_EscapeClearsAll);
        
        tp_ImGuiTreeNodeFlags->bindEnumValue("None", ImGuiTreeNodeFlags_None);
        tp_ImGuiTreeNodeFlags->bindEnumValue("Selected", ImGuiTreeNodeFlags_Selected);
        tp_ImGuiTreeNodeFlags->bindEnumValue("Framed", ImGuiTreeNodeFlags_Framed);
        tp_ImGuiTreeNodeFlags->bindEnumValue("AllowOverlap", ImGuiTreeNodeFlags_AllowOverlap);
        tp_ImGuiTreeNodeFlags->bindEnumValue("NoTreePushOnOpen", ImGuiTreeNodeFlags_NoTreePushOnOpen);
        tp_ImGuiTreeNodeFlags->bindEnumValue("NoAutoOpenOnLog", ImGuiTreeNodeFlags_NoAutoOpenOnLog);
        tp_ImGuiTreeNodeFlags->bindEnumValue("DefaultOpen", ImGuiTreeNodeFlags_DefaultOpen);
        tp_ImGuiTreeNodeFlags->bindEnumValue("OpenOnDoubleClick", ImGuiTreeNodeFlags_OpenOnDoubleClick);
        tp_ImGuiTreeNodeFlags->bindEnumValue("OpenOnArrow", ImGuiTreeNodeFlags_OpenOnArrow);
        tp_ImGuiTreeNodeFlags->bindEnumValue("Leaf", ImGuiTreeNodeFlags_Leaf);
        tp_ImGuiTreeNodeFlags->bindEnumValue("Bullet", ImGuiTreeNodeFlags_Bullet);
        tp_ImGuiTreeNodeFlags->bindEnumValue("FramePadding", ImGuiTreeNodeFlags_FramePadding);
        tp_ImGuiTreeNodeFlags->bindEnumValue("SpanAvailWidth", ImGuiTreeNodeFlags_SpanAvailWidth);
        tp_ImGuiTreeNodeFlags->bindEnumValue("SpanFullWidth", ImGuiTreeNodeFlags_SpanFullWidth);
        tp_ImGuiTreeNodeFlags->bindEnumValue("SpanAllColumns", ImGuiTreeNodeFlags_SpanAllColumns);
        tp_ImGuiTreeNodeFlags->bindEnumValue("NavLeftJumpsBackHere", ImGuiTreeNodeFlags_NavLeftJumpsBackHere);
        tp_ImGuiTreeNodeFlags->bindEnumValue("CollapsingHeader", ImGuiTreeNodeFlags_CollapsingHeader);
        tp_ImGuiTreeNodeFlags->bindEnumValue("AllowItemOverlap", ImGuiTreeNodeFlags_AllowItemOverlap);
        
        tp_ImGuiPopupFlags->bindEnumValue("None", ImGuiPopupFlags_None);
        tp_ImGuiPopupFlags->bindEnumValue("MouseButtonLeft", ImGuiPopupFlags_MouseButtonLeft);
        tp_ImGuiPopupFlags->bindEnumValue("MouseButtonRight", ImGuiPopupFlags_MouseButtonRight);
        tp_ImGuiPopupFlags->bindEnumValue("MouseButtonMiddle", ImGuiPopupFlags_MouseButtonMiddle);
        tp_ImGuiPopupFlags->bindEnumValue("MouseButtonMask_", ImGuiPopupFlags_MouseButtonMask_);
        tp_ImGuiPopupFlags->bindEnumValue("MouseButtonDefault_", ImGuiPopupFlags_MouseButtonDefault_);
        tp_ImGuiPopupFlags->bindEnumValue("NoReopen", ImGuiPopupFlags_NoReopen);
        tp_ImGuiPopupFlags->bindEnumValue("NoOpenOverExistingPopup", ImGuiPopupFlags_NoOpenOverExistingPopup);
        tp_ImGuiPopupFlags->bindEnumValue("NoOpenOverItems", ImGuiPopupFlags_NoOpenOverItems);
        tp_ImGuiPopupFlags->bindEnumValue("AnyPopupId", ImGuiPopupFlags_AnyPopupId);
        tp_ImGuiPopupFlags->bindEnumValue("AnyPopupLevel", ImGuiPopupFlags_AnyPopupLevel);
        tp_ImGuiPopupFlags->bindEnumValue("AnyPopup", ImGuiPopupFlags_AnyPopup);
        
        tp_ImGuiSelectableFlags->bindEnumValue("None", ImGuiSelectableFlags_None);
        tp_ImGuiSelectableFlags->bindEnumValue("DontClosePopups", ImGuiSelectableFlags_DontClosePopups);
        tp_ImGuiSelectableFlags->bindEnumValue("SpanAllColumns", ImGuiSelectableFlags_SpanAllColumns);
        tp_ImGuiSelectableFlags->bindEnumValue("AllowDoubleClick", ImGuiSelectableFlags_AllowDoubleClick);
        tp_ImGuiSelectableFlags->bindEnumValue("Disabled", ImGuiSelectableFlags_Disabled);
        tp_ImGuiSelectableFlags->bindEnumValue("AllowOverlap", ImGuiSelectableFlags_AllowOverlap);
        tp_ImGuiSelectableFlags->bindEnumValue("AllowItemOverlap", ImGuiSelectableFlags_AllowItemOverlap);
        
        tp_ImGuiComboFlags->bindEnumValue("None", ImGuiComboFlags_None);
        tp_ImGuiComboFlags->bindEnumValue("PopupAlignLeft", ImGuiComboFlags_PopupAlignLeft);
        tp_ImGuiComboFlags->bindEnumValue("HeightSmall", ImGuiComboFlags_HeightSmall);
        tp_ImGuiComboFlags->bindEnumValue("HeightRegular", ImGuiComboFlags_HeightRegular);
        tp_ImGuiComboFlags->bindEnumValue("HeightLarge", ImGuiComboFlags_HeightLarge);
        tp_ImGuiComboFlags->bindEnumValue("HeightLargest", ImGuiComboFlags_HeightLargest);
        tp_ImGuiComboFlags->bindEnumValue("NoArrowButton", ImGuiComboFlags_NoArrowButton);
        tp_ImGuiComboFlags->bindEnumValue("NoPreview", ImGuiComboFlags_NoPreview);
        tp_ImGuiComboFlags->bindEnumValue("WidthFitPreview", ImGuiComboFlags_WidthFitPreview);
        tp_ImGuiComboFlags->bindEnumValue("HeightMask_", ImGuiComboFlags_HeightMask_);
        
        tp_ImGuiTabBarFlags->bindEnumValue("None", ImGuiTabBarFlags_None);
        tp_ImGuiTabBarFlags->bindEnumValue("Reorderable", ImGuiTabBarFlags_Reorderable);
        tp_ImGuiTabBarFlags->bindEnumValue("AutoSelectNewTabs", ImGuiTabBarFlags_AutoSelectNewTabs);
        tp_ImGuiTabBarFlags->bindEnumValue("TabListPopupButton", ImGuiTabBarFlags_TabListPopupButton);
        tp_ImGuiTabBarFlags->bindEnumValue("NoCloseWithMiddleMouseButton", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton);
        tp_ImGuiTabBarFlags->bindEnumValue("NoTabListScrollingButtons", ImGuiTabBarFlags_NoTabListScrollingButtons);
        tp_ImGuiTabBarFlags->bindEnumValue("NoTooltip", ImGuiTabBarFlags_NoTooltip);
        tp_ImGuiTabBarFlags->bindEnumValue("FittingPolicyResizeDown", ImGuiTabBarFlags_FittingPolicyResizeDown);
        tp_ImGuiTabBarFlags->bindEnumValue("FittingPolicyScroll", ImGuiTabBarFlags_FittingPolicyScroll);
        tp_ImGuiTabBarFlags->bindEnumValue("FittingPolicyMask_", ImGuiTabBarFlags_FittingPolicyMask_);
        tp_ImGuiTabBarFlags->bindEnumValue("FittingPolicyDefault_", ImGuiTabBarFlags_FittingPolicyDefault_);
        
        tp_ImGuiTabItemFlags->bindEnumValue("None", ImGuiTabItemFlags_None);
        tp_ImGuiTabItemFlags->bindEnumValue("UnsavedDocument", ImGuiTabItemFlags_UnsavedDocument);
        tp_ImGuiTabItemFlags->bindEnumValue("SetSelected", ImGuiTabItemFlags_SetSelected);
        tp_ImGuiTabItemFlags->bindEnumValue("NoCloseWithMiddleMouseButton", ImGuiTabItemFlags_NoCloseWithMiddleMouseButton);
        tp_ImGuiTabItemFlags->bindEnumValue("NoPushId", ImGuiTabItemFlags_NoPushId);
        tp_ImGuiTabItemFlags->bindEnumValue("NoTooltip", ImGuiTabItemFlags_NoTooltip);
        tp_ImGuiTabItemFlags->bindEnumValue("NoReorder", ImGuiTabItemFlags_NoReorder);
        tp_ImGuiTabItemFlags->bindEnumValue("Leading", ImGuiTabItemFlags_Leading);
        tp_ImGuiTabItemFlags->bindEnumValue("Trailing", ImGuiTabItemFlags_Trailing);
        tp_ImGuiTabItemFlags->bindEnumValue("NoAssumedClosure", ImGuiTabItemFlags_NoAssumedClosure);
        
        tp_ImGuiFocusedFlags->bindEnumValue("None", ImGuiFocusedFlags_None);
        tp_ImGuiFocusedFlags->bindEnumValue("ChildWindows", ImGuiFocusedFlags_ChildWindows);
        tp_ImGuiFocusedFlags->bindEnumValue("RootWindow", ImGuiFocusedFlags_RootWindow);
        tp_ImGuiFocusedFlags->bindEnumValue("AnyWindow", ImGuiFocusedFlags_AnyWindow);
        tp_ImGuiFocusedFlags->bindEnumValue("NoPopupHierarchy", ImGuiFocusedFlags_NoPopupHierarchy);
        tp_ImGuiFocusedFlags->bindEnumValue("RootAndChildWindows", ImGuiFocusedFlags_RootAndChildWindows);
        
        tp_ImGuiHoveredFlags->bindEnumValue("None", ImGuiHoveredFlags_None);
        tp_ImGuiHoveredFlags->bindEnumValue("ChildWindows", ImGuiHoveredFlags_ChildWindows);
        tp_ImGuiHoveredFlags->bindEnumValue("RootWindow", ImGuiHoveredFlags_RootWindow);
        tp_ImGuiHoveredFlags->bindEnumValue("AnyWindow", ImGuiHoveredFlags_AnyWindow);
        tp_ImGuiHoveredFlags->bindEnumValue("NoPopupHierarchy", ImGuiHoveredFlags_NoPopupHierarchy);
        tp_ImGuiHoveredFlags->bindEnumValue("AllowWhenBlockedByPopup", ImGuiHoveredFlags_AllowWhenBlockedByPopup);
        tp_ImGuiHoveredFlags->bindEnumValue("AllowWhenBlockedByActiveItem", ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
        tp_ImGuiHoveredFlags->bindEnumValue("AllowWhenOverlappedByItem", ImGuiHoveredFlags_AllowWhenOverlappedByItem);
        tp_ImGuiHoveredFlags->bindEnumValue("AllowWhenOverlappedByWindow", ImGuiHoveredFlags_AllowWhenOverlappedByWindow);
        tp_ImGuiHoveredFlags->bindEnumValue("AllowWhenDisabled", ImGuiHoveredFlags_AllowWhenDisabled);
        tp_ImGuiHoveredFlags->bindEnumValue("NoNavOverride", ImGuiHoveredFlags_NoNavOverride);
        tp_ImGuiHoveredFlags->bindEnumValue("AllowWhenOverlapped", ImGuiHoveredFlags_AllowWhenOverlapped);
        tp_ImGuiHoveredFlags->bindEnumValue("RectOnly", ImGuiHoveredFlags_RectOnly);
        tp_ImGuiHoveredFlags->bindEnumValue("RootAndChildWindows", ImGuiHoveredFlags_RootAndChildWindows);
        tp_ImGuiHoveredFlags->bindEnumValue("ForTooltip", ImGuiHoveredFlags_ForTooltip);
        tp_ImGuiHoveredFlags->bindEnumValue("Stationary", ImGuiHoveredFlags_Stationary);
        tp_ImGuiHoveredFlags->bindEnumValue("DelayNone", ImGuiHoveredFlags_DelayNone);
        tp_ImGuiHoveredFlags->bindEnumValue("DelayShort", ImGuiHoveredFlags_DelayShort);
        tp_ImGuiHoveredFlags->bindEnumValue("DelayNormal", ImGuiHoveredFlags_DelayNormal);
        tp_ImGuiHoveredFlags->bindEnumValue("NoSharedDelay", ImGuiHoveredFlags_NoSharedDelay);
        
        tp_ImGuiDragDropFlags->bindEnumValue("None", ImGuiDragDropFlags_None);
        tp_ImGuiDragDropFlags->bindEnumValue("SourceNoPreviewTooltip", ImGuiDragDropFlags_SourceNoPreviewTooltip);
        tp_ImGuiDragDropFlags->bindEnumValue("SourceNoDisableHover", ImGuiDragDropFlags_SourceNoDisableHover);
        tp_ImGuiDragDropFlags->bindEnumValue("SourceNoHoldToOpenOthers", ImGuiDragDropFlags_SourceNoHoldToOpenOthers);
        tp_ImGuiDragDropFlags->bindEnumValue("SourceAllowNullID", ImGuiDragDropFlags_SourceAllowNullID);
        tp_ImGuiDragDropFlags->bindEnumValue("SourceExtern", ImGuiDragDropFlags_SourceExtern);
        tp_ImGuiDragDropFlags->bindEnumValue("SourceAutoExpirePayload", ImGuiDragDropFlags_SourceAutoExpirePayload);
        tp_ImGuiDragDropFlags->bindEnumValue("AcceptBeforeDelivery", ImGuiDragDropFlags_AcceptBeforeDelivery);
        tp_ImGuiDragDropFlags->bindEnumValue("AcceptNoDrawDefaultRect", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);
        tp_ImGuiDragDropFlags->bindEnumValue("AcceptNoPreviewTooltip", ImGuiDragDropFlags_AcceptNoPreviewTooltip);
        tp_ImGuiDragDropFlags->bindEnumValue("AcceptPeekOnly", ImGuiDragDropFlags_AcceptPeekOnly);
        
        tp_ImGuiDataType->bindEnumValue("S8", ImGuiDataType_S8);
        tp_ImGuiDataType->bindEnumValue("U8", ImGuiDataType_U8);
        tp_ImGuiDataType->bindEnumValue("S16", ImGuiDataType_S16);
        tp_ImGuiDataType->bindEnumValue("U16", ImGuiDataType_U16);
        tp_ImGuiDataType->bindEnumValue("S32", ImGuiDataType_S32);
        tp_ImGuiDataType->bindEnumValue("U32", ImGuiDataType_U32);
        tp_ImGuiDataType->bindEnumValue("S64", ImGuiDataType_S64);
        tp_ImGuiDataType->bindEnumValue("U64", ImGuiDataType_U64);
        tp_ImGuiDataType->bindEnumValue("Float", ImGuiDataType_Float);
        tp_ImGuiDataType->bindEnumValue("Double", ImGuiDataType_Double);
        
        tp_ImGuiDir->bindEnumValue("None", ImGuiDir_None);
        tp_ImGuiDir->bindEnumValue("Left", ImGuiDir_Left);
        tp_ImGuiDir->bindEnumValue("Right", ImGuiDir_Right);
        tp_ImGuiDir->bindEnumValue("Up", ImGuiDir_Up);
        tp_ImGuiDir->bindEnumValue("Down", ImGuiDir_Down);
        
        tp_ImGuiSortDirection->bindEnumValue("None", ImGuiSortDirection_None);
        tp_ImGuiSortDirection->bindEnumValue("Ascending", ImGuiSortDirection_Ascending);
        tp_ImGuiSortDirection->bindEnumValue("Descending", ImGuiSortDirection_Descending);
        
        tp_ImGuiKey->bindEnumValue("None", ImGuiKey_None);
        tp_ImGuiKey->bindEnumValue("Tab", ImGuiKey_Tab);
        tp_ImGuiKey->bindEnumValue("LeftArrow", ImGuiKey_LeftArrow);
        tp_ImGuiKey->bindEnumValue("RightArrow", ImGuiKey_RightArrow);
        tp_ImGuiKey->bindEnumValue("UpArrow", ImGuiKey_UpArrow);
        tp_ImGuiKey->bindEnumValue("DownArrow", ImGuiKey_DownArrow);
        tp_ImGuiKey->bindEnumValue("PageUp", ImGuiKey_PageUp);
        tp_ImGuiKey->bindEnumValue("PageDown", ImGuiKey_PageDown);
        tp_ImGuiKey->bindEnumValue("Home", ImGuiKey_Home);
        tp_ImGuiKey->bindEnumValue("End", ImGuiKey_End);
        tp_ImGuiKey->bindEnumValue("Insert", ImGuiKey_Insert);
        tp_ImGuiKey->bindEnumValue("Delete", ImGuiKey_Delete);
        tp_ImGuiKey->bindEnumValue("Backspace", ImGuiKey_Backspace);
        tp_ImGuiKey->bindEnumValue("Space", ImGuiKey_Space);
        tp_ImGuiKey->bindEnumValue("Enter", ImGuiKey_Enter);
        tp_ImGuiKey->bindEnumValue("Escape", ImGuiKey_Escape);
        tp_ImGuiKey->bindEnumValue("LeftCtrl", ImGuiKey_LeftCtrl);
        tp_ImGuiKey->bindEnumValue("LeftShift", ImGuiKey_LeftShift);
        tp_ImGuiKey->bindEnumValue("LeftAlt", ImGuiKey_LeftAlt);
        tp_ImGuiKey->bindEnumValue("LeftSuper", ImGuiKey_LeftSuper);
        tp_ImGuiKey->bindEnumValue("RightCtrl", ImGuiKey_RightCtrl);
        tp_ImGuiKey->bindEnumValue("RightShift", ImGuiKey_RightShift);
        tp_ImGuiKey->bindEnumValue("RightAlt", ImGuiKey_RightAlt);
        tp_ImGuiKey->bindEnumValue("RightSuper", ImGuiKey_RightSuper);
        tp_ImGuiKey->bindEnumValue("Menu", ImGuiKey_Menu);
        tp_ImGuiKey->bindEnumValue("Num0", ImGuiKey_0);
        tp_ImGuiKey->bindEnumValue("Num1", ImGuiKey_1);
        tp_ImGuiKey->bindEnumValue("Num2", ImGuiKey_2);
        tp_ImGuiKey->bindEnumValue("Num3", ImGuiKey_3);
        tp_ImGuiKey->bindEnumValue("Num4", ImGuiKey_4);
        tp_ImGuiKey->bindEnumValue("Num5", ImGuiKey_5);
        tp_ImGuiKey->bindEnumValue("Num6", ImGuiKey_6);
        tp_ImGuiKey->bindEnumValue("Num7", ImGuiKey_7);
        tp_ImGuiKey->bindEnumValue("Num8", ImGuiKey_8);
        tp_ImGuiKey->bindEnumValue("Num9", ImGuiKey_9);
        tp_ImGuiKey->bindEnumValue("A", ImGuiKey_A);
        tp_ImGuiKey->bindEnumValue("B", ImGuiKey_B);
        tp_ImGuiKey->bindEnumValue("C", ImGuiKey_C);
        tp_ImGuiKey->bindEnumValue("D", ImGuiKey_D);
        tp_ImGuiKey->bindEnumValue("E", ImGuiKey_E);
        tp_ImGuiKey->bindEnumValue("F", ImGuiKey_F);
        tp_ImGuiKey->bindEnumValue("G", ImGuiKey_G);
        tp_ImGuiKey->bindEnumValue("H", ImGuiKey_H);
        tp_ImGuiKey->bindEnumValue("I", ImGuiKey_I);
        tp_ImGuiKey->bindEnumValue("J", ImGuiKey_J);
        tp_ImGuiKey->bindEnumValue("K", ImGuiKey_K);
        tp_ImGuiKey->bindEnumValue("L", ImGuiKey_L);
        tp_ImGuiKey->bindEnumValue("M", ImGuiKey_M);
        tp_ImGuiKey->bindEnumValue("N", ImGuiKey_N);
        tp_ImGuiKey->bindEnumValue("O", ImGuiKey_O);
        tp_ImGuiKey->bindEnumValue("P", ImGuiKey_P);
        tp_ImGuiKey->bindEnumValue("Q", ImGuiKey_Q);
        tp_ImGuiKey->bindEnumValue("R", ImGuiKey_R);
        tp_ImGuiKey->bindEnumValue("S", ImGuiKey_S);
        tp_ImGuiKey->bindEnumValue("T", ImGuiKey_T);
        tp_ImGuiKey->bindEnumValue("U", ImGuiKey_U);
        tp_ImGuiKey->bindEnumValue("V", ImGuiKey_V);
        tp_ImGuiKey->bindEnumValue("W", ImGuiKey_W);
        tp_ImGuiKey->bindEnumValue("X", ImGuiKey_X);
        tp_ImGuiKey->bindEnumValue("Y", ImGuiKey_Y);
        tp_ImGuiKey->bindEnumValue("Z", ImGuiKey_Z);
        tp_ImGuiKey->bindEnumValue("F1", ImGuiKey_F1);
        tp_ImGuiKey->bindEnumValue("F2", ImGuiKey_F2);
        tp_ImGuiKey->bindEnumValue("F3", ImGuiKey_F3);
        tp_ImGuiKey->bindEnumValue("F4", ImGuiKey_F4);
        tp_ImGuiKey->bindEnumValue("F5", ImGuiKey_F5);
        tp_ImGuiKey->bindEnumValue("F6", ImGuiKey_F6);
        tp_ImGuiKey->bindEnumValue("F7", ImGuiKey_F7);
        tp_ImGuiKey->bindEnumValue("F8", ImGuiKey_F8);
        tp_ImGuiKey->bindEnumValue("F9", ImGuiKey_F9);
        tp_ImGuiKey->bindEnumValue("F10", ImGuiKey_F10);
        tp_ImGuiKey->bindEnumValue("F11", ImGuiKey_F11);
        tp_ImGuiKey->bindEnumValue("F12", ImGuiKey_F12);
        tp_ImGuiKey->bindEnumValue("F13", ImGuiKey_F13);
        tp_ImGuiKey->bindEnumValue("F14", ImGuiKey_F14);
        tp_ImGuiKey->bindEnumValue("F15", ImGuiKey_F15);
        tp_ImGuiKey->bindEnumValue("F16", ImGuiKey_F16);
        tp_ImGuiKey->bindEnumValue("F17", ImGuiKey_F17);
        tp_ImGuiKey->bindEnumValue("F18", ImGuiKey_F18);
        tp_ImGuiKey->bindEnumValue("F19", ImGuiKey_F19);
        tp_ImGuiKey->bindEnumValue("F20", ImGuiKey_F20);
        tp_ImGuiKey->bindEnumValue("F21", ImGuiKey_F21);
        tp_ImGuiKey->bindEnumValue("F22", ImGuiKey_F22);
        tp_ImGuiKey->bindEnumValue("F23", ImGuiKey_F23);
        tp_ImGuiKey->bindEnumValue("F24", ImGuiKey_F24);
        tp_ImGuiKey->bindEnumValue("Apostrophe", ImGuiKey_Apostrophe);
        tp_ImGuiKey->bindEnumValue("Comma", ImGuiKey_Comma);
        tp_ImGuiKey->bindEnumValue("Minus", ImGuiKey_Minus);
        tp_ImGuiKey->bindEnumValue("Period", ImGuiKey_Period);
        tp_ImGuiKey->bindEnumValue("Slash", ImGuiKey_Slash);
        tp_ImGuiKey->bindEnumValue("Semicolon", ImGuiKey_Semicolon);
        tp_ImGuiKey->bindEnumValue("Equal", ImGuiKey_Equal);
        tp_ImGuiKey->bindEnumValue("LeftBracket", ImGuiKey_LeftBracket);
        tp_ImGuiKey->bindEnumValue("Backslash", ImGuiKey_Backslash);
        tp_ImGuiKey->bindEnumValue("RightBracket", ImGuiKey_RightBracket);
        tp_ImGuiKey->bindEnumValue("GraveAccent", ImGuiKey_GraveAccent);
        tp_ImGuiKey->bindEnumValue("CapsLock", ImGuiKey_CapsLock);
        tp_ImGuiKey->bindEnumValue("ScrollLock", ImGuiKey_ScrollLock);
        tp_ImGuiKey->bindEnumValue("NumLock", ImGuiKey_NumLock);
        tp_ImGuiKey->bindEnumValue("PrintScreen", ImGuiKey_PrintScreen);
        tp_ImGuiKey->bindEnumValue("Pause", ImGuiKey_Pause);
        tp_ImGuiKey->bindEnumValue("Keypad0", ImGuiKey_Keypad0);
        tp_ImGuiKey->bindEnumValue("Keypad1", ImGuiKey_Keypad1);
        tp_ImGuiKey->bindEnumValue("Keypad2", ImGuiKey_Keypad2);
        tp_ImGuiKey->bindEnumValue("Keypad3", ImGuiKey_Keypad3);
        tp_ImGuiKey->bindEnumValue("Keypad4", ImGuiKey_Keypad4);
        tp_ImGuiKey->bindEnumValue("Keypad5", ImGuiKey_Keypad5);
        tp_ImGuiKey->bindEnumValue("Keypad6", ImGuiKey_Keypad6);
        tp_ImGuiKey->bindEnumValue("Keypad7", ImGuiKey_Keypad7);
        tp_ImGuiKey->bindEnumValue("Keypad8", ImGuiKey_Keypad8);
        tp_ImGuiKey->bindEnumValue("Keypad9", ImGuiKey_Keypad9);
        tp_ImGuiKey->bindEnumValue("KeypadDecimal", ImGuiKey_KeypadDecimal);
        tp_ImGuiKey->bindEnumValue("KeypadDivide", ImGuiKey_KeypadDivide);
        tp_ImGuiKey->bindEnumValue("KeypadMultiply", ImGuiKey_KeypadMultiply);
        tp_ImGuiKey->bindEnumValue("KeypadSubtract", ImGuiKey_KeypadSubtract);
        tp_ImGuiKey->bindEnumValue("KeypadAdd", ImGuiKey_KeypadAdd);
        tp_ImGuiKey->bindEnumValue("KeypadEnter", ImGuiKey_KeypadEnter);
        tp_ImGuiKey->bindEnumValue("KeypadEqual", ImGuiKey_KeypadEqual);
        tp_ImGuiKey->bindEnumValue("AppBack", ImGuiKey_AppBack);
        tp_ImGuiKey->bindEnumValue("AppForward", ImGuiKey_AppForward);
        tp_ImGuiKey->bindEnumValue("GamepadStart", ImGuiKey_GamepadStart);
        tp_ImGuiKey->bindEnumValue("GamepadBack", ImGuiKey_GamepadBack);
        tp_ImGuiKey->bindEnumValue("GamepadFaceLeft", ImGuiKey_GamepadFaceLeft);
        tp_ImGuiKey->bindEnumValue("GamepadFaceRight", ImGuiKey_GamepadFaceRight);
        tp_ImGuiKey->bindEnumValue("GamepadFaceUp", ImGuiKey_GamepadFaceUp);
        tp_ImGuiKey->bindEnumValue("GamepadFaceDown", ImGuiKey_GamepadFaceDown);
        tp_ImGuiKey->bindEnumValue("GamepadDpadLeft", ImGuiKey_GamepadDpadLeft);
        tp_ImGuiKey->bindEnumValue("GamepadDpadRight", ImGuiKey_GamepadDpadRight);
        tp_ImGuiKey->bindEnumValue("GamepadDpadUp", ImGuiKey_GamepadDpadUp);
        tp_ImGuiKey->bindEnumValue("GamepadDpadDown", ImGuiKey_GamepadDpadDown);
        tp_ImGuiKey->bindEnumValue("GamepadL1", ImGuiKey_GamepadL1);
        tp_ImGuiKey->bindEnumValue("GamepadR1", ImGuiKey_GamepadR1);
        tp_ImGuiKey->bindEnumValue("GamepadL2", ImGuiKey_GamepadL2);
        tp_ImGuiKey->bindEnumValue("GamepadR2", ImGuiKey_GamepadR2);
        tp_ImGuiKey->bindEnumValue("GamepadL3", ImGuiKey_GamepadL3);
        tp_ImGuiKey->bindEnumValue("GamepadR3", ImGuiKey_GamepadR3);
        tp_ImGuiKey->bindEnumValue("GamepadLStickLeft", ImGuiKey_GamepadLStickLeft);
        tp_ImGuiKey->bindEnumValue("GamepadLStickRight", ImGuiKey_GamepadLStickRight);
        tp_ImGuiKey->bindEnumValue("GamepadLStickUp", ImGuiKey_GamepadLStickUp);
        tp_ImGuiKey->bindEnumValue("GamepadLStickDown", ImGuiKey_GamepadLStickDown);
        tp_ImGuiKey->bindEnumValue("GamepadRStickLeft", ImGuiKey_GamepadRStickLeft);
        tp_ImGuiKey->bindEnumValue("GamepadRStickRight", ImGuiKey_GamepadRStickRight);
        tp_ImGuiKey->bindEnumValue("GamepadRStickUp", ImGuiKey_GamepadRStickUp);
        tp_ImGuiKey->bindEnumValue("GamepadRStickDown", ImGuiKey_GamepadRStickDown);
        tp_ImGuiKey->bindEnumValue("MouseLeft", ImGuiKey_MouseLeft);
        tp_ImGuiKey->bindEnumValue("MouseRight", ImGuiKey_MouseRight);
        tp_ImGuiKey->bindEnumValue("MouseMiddle", ImGuiKey_MouseMiddle);
        tp_ImGuiKey->bindEnumValue("MouseX1", ImGuiKey_MouseX1);
        tp_ImGuiKey->bindEnumValue("MouseX2", ImGuiKey_MouseX2);
        tp_ImGuiKey->bindEnumValue("MouseWheelX", ImGuiKey_MouseWheelX);
        tp_ImGuiKey->bindEnumValue("MouseWheelY", ImGuiKey_MouseWheelY);
        tp_ImGuiKey->bindEnumValue("ReservedForModCtrl", ImGuiKey_ReservedForModCtrl);
        tp_ImGuiKey->bindEnumValue("ReservedForModShift", ImGuiKey_ReservedForModShift);
        tp_ImGuiKey->bindEnumValue("ReservedForModAlt", ImGuiKey_ReservedForModAlt);
        tp_ImGuiKey->bindEnumValue("ReservedForModSuper", ImGuiKey_ReservedForModSuper);
        tp_ImGuiKey->bindEnumValue("Ctrl", ImGuiMod_Ctrl);
        tp_ImGuiKey->bindEnumValue("Shift", ImGuiMod_Shift);
        tp_ImGuiKey->bindEnumValue("Alt", ImGuiMod_Alt);
        tp_ImGuiKey->bindEnumValue("Super", ImGuiMod_Super);
        tp_ImGuiKey->bindEnumValue("Shortcut", ImGuiMod_Shortcut);
        tp_ImGuiKey->bindEnumValue("Mask_", ImGuiMod_Mask_);
        tp_ImGuiKey->bindEnumValue("ModCtrl", ImGuiKey_ModCtrl);
        tp_ImGuiKey->bindEnumValue("ModShift", ImGuiKey_ModShift);
        tp_ImGuiKey->bindEnumValue("ModAlt", ImGuiKey_ModAlt);
        tp_ImGuiKey->bindEnumValue("ModSuper", ImGuiKey_ModSuper);
        
        tp_ImGuiNavInput->bindEnumValue("Activate", ImGuiNavInput_Activate);
        tp_ImGuiNavInput->bindEnumValue("Cancel", ImGuiNavInput_Cancel);
        tp_ImGuiNavInput->bindEnumValue("Input", ImGuiNavInput_Input);
        tp_ImGuiNavInput->bindEnumValue("Menu", ImGuiNavInput_Menu);
        tp_ImGuiNavInput->bindEnumValue("DpadLeft", ImGuiNavInput_DpadLeft);
        tp_ImGuiNavInput->bindEnumValue("DpadRight", ImGuiNavInput_DpadRight);
        tp_ImGuiNavInput->bindEnumValue("DpadUp", ImGuiNavInput_DpadUp);
        tp_ImGuiNavInput->bindEnumValue("DpadDown", ImGuiNavInput_DpadDown);
        tp_ImGuiNavInput->bindEnumValue("LStickLeft", ImGuiNavInput_LStickLeft);
        tp_ImGuiNavInput->bindEnumValue("LStickRight", ImGuiNavInput_LStickRight);
        tp_ImGuiNavInput->bindEnumValue("LStickUp", ImGuiNavInput_LStickUp);
        tp_ImGuiNavInput->bindEnumValue("LStickDown", ImGuiNavInput_LStickDown);
        tp_ImGuiNavInput->bindEnumValue("FocusPrev", ImGuiNavInput_FocusPrev);
        tp_ImGuiNavInput->bindEnumValue("FocusNext", ImGuiNavInput_FocusNext);
        tp_ImGuiNavInput->bindEnumValue("TweakSlow", ImGuiNavInput_TweakSlow);
        tp_ImGuiNavInput->bindEnumValue("TweakFast", ImGuiNavInput_TweakFast);
        
        tp_ImGuiConfigFlags->bindEnumValue("None", ImGuiConfigFlags_None);
        tp_ImGuiConfigFlags->bindEnumValue("NavEnableKeyboard", ImGuiConfigFlags_NavEnableKeyboard);
        tp_ImGuiConfigFlags->bindEnumValue("NavEnableGamepad", ImGuiConfigFlags_NavEnableGamepad);
        tp_ImGuiConfigFlags->bindEnumValue("NavEnableSetMousePos", ImGuiConfigFlags_NavEnableSetMousePos);
        tp_ImGuiConfigFlags->bindEnumValue("NavNoCaptureKeyboard", ImGuiConfigFlags_NavNoCaptureKeyboard);
        tp_ImGuiConfigFlags->bindEnumValue("NoMouse", ImGuiConfigFlags_NoMouse);
        tp_ImGuiConfigFlags->bindEnumValue("NoMouseCursorChange", ImGuiConfigFlags_NoMouseCursorChange);
        tp_ImGuiConfigFlags->bindEnumValue("IsSRGB", ImGuiConfigFlags_IsSRGB);
        tp_ImGuiConfigFlags->bindEnumValue("IsTouchScreen", ImGuiConfigFlags_IsTouchScreen);
        tp_ImGuiBackendFlags->bindEnumValue("None", ImGuiBackendFlags_None);
        tp_ImGuiBackendFlags->bindEnumValue("HasGamepad", ImGuiBackendFlags_HasGamepad);
        tp_ImGuiBackendFlags->bindEnumValue("HasMouseCursors", ImGuiBackendFlags_HasMouseCursors);
        tp_ImGuiBackendFlags->bindEnumValue("HasSetMousePos", ImGuiBackendFlags_HasSetMousePos);
        tp_ImGuiBackendFlags->bindEnumValue("RendererHasVtxOffset", ImGuiBackendFlags_RendererHasVtxOffset);
        
        tp_ImGuiCol->bindEnumValue("Text", ImGuiCol_Text);
        tp_ImGuiCol->bindEnumValue("TextDisabled", ImGuiCol_TextDisabled);
        tp_ImGuiCol->bindEnumValue("WindowBg", ImGuiCol_WindowBg);
        tp_ImGuiCol->bindEnumValue("ChildBg", ImGuiCol_ChildBg);
        tp_ImGuiCol->bindEnumValue("PopupBg", ImGuiCol_PopupBg);
        tp_ImGuiCol->bindEnumValue("Border", ImGuiCol_Border);
        tp_ImGuiCol->bindEnumValue("BorderShadow", ImGuiCol_BorderShadow);
        tp_ImGuiCol->bindEnumValue("FrameBg", ImGuiCol_FrameBg);
        tp_ImGuiCol->bindEnumValue("FrameBgHovered", ImGuiCol_FrameBgHovered);
        tp_ImGuiCol->bindEnumValue("FrameBgActive", ImGuiCol_FrameBgActive);
        tp_ImGuiCol->bindEnumValue("TitleBg", ImGuiCol_TitleBg);
        tp_ImGuiCol->bindEnumValue("TitleBgActive", ImGuiCol_TitleBgActive);
        tp_ImGuiCol->bindEnumValue("TitleBgCollapsed", ImGuiCol_TitleBgCollapsed);
        tp_ImGuiCol->bindEnumValue("MenuBarBg", ImGuiCol_MenuBarBg);
        tp_ImGuiCol->bindEnumValue("ScrollbarBg", ImGuiCol_ScrollbarBg);
        tp_ImGuiCol->bindEnumValue("ScrollbarGrab", ImGuiCol_ScrollbarGrab);
        tp_ImGuiCol->bindEnumValue("ScrollbarGrabHovered", ImGuiCol_ScrollbarGrabHovered);
        tp_ImGuiCol->bindEnumValue("ScrollbarGrabActive", ImGuiCol_ScrollbarGrabActive);
        tp_ImGuiCol->bindEnumValue("CheckMark", ImGuiCol_CheckMark);
        tp_ImGuiCol->bindEnumValue("SliderGrab", ImGuiCol_SliderGrab);
        tp_ImGuiCol->bindEnumValue("SliderGrabActive", ImGuiCol_SliderGrabActive);
        tp_ImGuiCol->bindEnumValue("Button", ImGuiCol_Button);
        tp_ImGuiCol->bindEnumValue("ButtonHovered", ImGuiCol_ButtonHovered);
        tp_ImGuiCol->bindEnumValue("ButtonActive", ImGuiCol_ButtonActive);
        tp_ImGuiCol->bindEnumValue("Header", ImGuiCol_Header);
        tp_ImGuiCol->bindEnumValue("HeaderHovered", ImGuiCol_HeaderHovered);
        tp_ImGuiCol->bindEnumValue("HeaderActive", ImGuiCol_HeaderActive);
        tp_ImGuiCol->bindEnumValue("Separator", ImGuiCol_Separator);
        tp_ImGuiCol->bindEnumValue("SeparatorHovered", ImGuiCol_SeparatorHovered);
        tp_ImGuiCol->bindEnumValue("SeparatorActive", ImGuiCol_SeparatorActive);
        tp_ImGuiCol->bindEnumValue("ResizeGrip", ImGuiCol_ResizeGrip);
        tp_ImGuiCol->bindEnumValue("ResizeGripHovered", ImGuiCol_ResizeGripHovered);
        tp_ImGuiCol->bindEnumValue("ResizeGripActive", ImGuiCol_ResizeGripActive);
        tp_ImGuiCol->bindEnumValue("Tab", ImGuiCol_Tab);
        tp_ImGuiCol->bindEnumValue("TabHovered", ImGuiCol_TabHovered);
        tp_ImGuiCol->bindEnumValue("TabActive", ImGuiCol_TabActive);
        tp_ImGuiCol->bindEnumValue("TabUnfocused", ImGuiCol_TabUnfocused);
        tp_ImGuiCol->bindEnumValue("TabUnfocusedActive", ImGuiCol_TabUnfocusedActive);
        tp_ImGuiCol->bindEnumValue("PlotLines", ImGuiCol_PlotLines);
        tp_ImGuiCol->bindEnumValue("PlotLinesHovered", ImGuiCol_PlotLinesHovered);
        tp_ImGuiCol->bindEnumValue("PlotHistogram", ImGuiCol_PlotHistogram);
        tp_ImGuiCol->bindEnumValue("PlotHistogramHovered", ImGuiCol_PlotHistogramHovered);
        tp_ImGuiCol->bindEnumValue("TableHeaderBg", ImGuiCol_TableHeaderBg);
        tp_ImGuiCol->bindEnumValue("TableBorderStrong", ImGuiCol_TableBorderStrong);
        tp_ImGuiCol->bindEnumValue("TableBorderLight", ImGuiCol_TableBorderLight);
        tp_ImGuiCol->bindEnumValue("TableRowBg", ImGuiCol_TableRowBg);
        tp_ImGuiCol->bindEnumValue("TableRowBgAlt", ImGuiCol_TableRowBgAlt);
        tp_ImGuiCol->bindEnumValue("TextSelectedBg", ImGuiCol_TextSelectedBg);
        tp_ImGuiCol->bindEnumValue("DragDropTarget", ImGuiCol_DragDropTarget);
        tp_ImGuiCol->bindEnumValue("NavHighlight", ImGuiCol_NavHighlight);
        tp_ImGuiCol->bindEnumValue("NavWindowingHighlight", ImGuiCol_NavWindowingHighlight);
        tp_ImGuiCol->bindEnumValue("NavWindowingDimBg", ImGuiCol_NavWindowingDimBg);
        tp_ImGuiCol->bindEnumValue("ModalWindowDimBg", ImGuiCol_ModalWindowDimBg);
        
        tp_ImGuiStyleVar->bindEnumValue("Alpha", ImGuiStyleVar_Alpha);
        tp_ImGuiStyleVar->bindEnumValue("DisabledAlpha", ImGuiStyleVar_DisabledAlpha);
        tp_ImGuiStyleVar->bindEnumValue("WindowPadding", ImGuiStyleVar_WindowPadding);
        tp_ImGuiStyleVar->bindEnumValue("WindowRounding", ImGuiStyleVar_WindowRounding);
        tp_ImGuiStyleVar->bindEnumValue("WindowBorderSize", ImGuiStyleVar_WindowBorderSize);
        tp_ImGuiStyleVar->bindEnumValue("WindowMinSize", ImGuiStyleVar_WindowMinSize);
        tp_ImGuiStyleVar->bindEnumValue("WindowTitleAlign", ImGuiStyleVar_WindowTitleAlign);
        tp_ImGuiStyleVar->bindEnumValue("ChildRounding", ImGuiStyleVar_ChildRounding);
        tp_ImGuiStyleVar->bindEnumValue("ChildBorderSize", ImGuiStyleVar_ChildBorderSize);
        tp_ImGuiStyleVar->bindEnumValue("PopupRounding", ImGuiStyleVar_PopupRounding);
        tp_ImGuiStyleVar->bindEnumValue("PopupBorderSize", ImGuiStyleVar_PopupBorderSize);
        tp_ImGuiStyleVar->bindEnumValue("FramePadding", ImGuiStyleVar_FramePadding);
        tp_ImGuiStyleVar->bindEnumValue("FrameRounding", ImGuiStyleVar_FrameRounding);
        tp_ImGuiStyleVar->bindEnumValue("FrameBorderSize", ImGuiStyleVar_FrameBorderSize);
        tp_ImGuiStyleVar->bindEnumValue("ItemSpacing", ImGuiStyleVar_ItemSpacing);
        tp_ImGuiStyleVar->bindEnumValue("ItemInnerSpacing", ImGuiStyleVar_ItemInnerSpacing);
        tp_ImGuiStyleVar->bindEnumValue("IndentSpacing", ImGuiStyleVar_IndentSpacing);
        tp_ImGuiStyleVar->bindEnumValue("CellPadding", ImGuiStyleVar_CellPadding);
        tp_ImGuiStyleVar->bindEnumValue("ScrollbarSize", ImGuiStyleVar_ScrollbarSize);
        tp_ImGuiStyleVar->bindEnumValue("ScrollbarRounding", ImGuiStyleVar_ScrollbarRounding);
        tp_ImGuiStyleVar->bindEnumValue("GrabMinSize", ImGuiStyleVar_GrabMinSize);
        tp_ImGuiStyleVar->bindEnumValue("GrabRounding", ImGuiStyleVar_GrabRounding);
        tp_ImGuiStyleVar->bindEnumValue("TabRounding", ImGuiStyleVar_TabRounding);
        tp_ImGuiStyleVar->bindEnumValue("TabBorderSize", ImGuiStyleVar_TabBorderSize);
        tp_ImGuiStyleVar->bindEnumValue("TabBarBorderSize", ImGuiStyleVar_TabBarBorderSize);
        tp_ImGuiStyleVar->bindEnumValue("TableAngledHeadersAngle", ImGuiStyleVar_TableAngledHeadersAngle);
        tp_ImGuiStyleVar->bindEnumValue("ButtonTextAlign", ImGuiStyleVar_ButtonTextAlign);
        tp_ImGuiStyleVar->bindEnumValue("SelectableTextAlign", ImGuiStyleVar_SelectableTextAlign);
        tp_ImGuiStyleVar->bindEnumValue("SeparatorTextBorderSize", ImGuiStyleVar_SeparatorTextBorderSize);
        tp_ImGuiStyleVar->bindEnumValue("SeparatorTextAlign", ImGuiStyleVar_SeparatorTextAlign);
        tp_ImGuiStyleVar->bindEnumValue("SeparatorTextPadding", ImGuiStyleVar_SeparatorTextPadding);
        
        tp_ImGuiButtonFlags->bindEnumValue("None", ImGuiButtonFlags_None);
        tp_ImGuiButtonFlags->bindEnumValue("MouseButtonLeft", ImGuiButtonFlags_MouseButtonLeft);
        tp_ImGuiButtonFlags->bindEnumValue("MouseButtonRight", ImGuiButtonFlags_MouseButtonRight);
        tp_ImGuiButtonFlags->bindEnumValue("MouseButtonMiddle", ImGuiButtonFlags_MouseButtonMiddle);
        tp_ImGuiButtonFlags->bindEnumValue("MouseButtonMask_", ImGuiButtonFlags_MouseButtonMask_);
        tp_ImGuiButtonFlags->bindEnumValue("MouseButtonDefault_", ImGuiButtonFlags_MouseButtonDefault_);
        
        tp_ImGuiColorEditFlags->bindEnumValue("None", ImGuiColorEditFlags_None);
        tp_ImGuiColorEditFlags->bindEnumValue("NoAlpha", ImGuiColorEditFlags_NoAlpha);
        tp_ImGuiColorEditFlags->bindEnumValue("NoPicker", ImGuiColorEditFlags_NoPicker);
        tp_ImGuiColorEditFlags->bindEnumValue("NoOptions", ImGuiColorEditFlags_NoOptions);
        tp_ImGuiColorEditFlags->bindEnumValue("NoSmallPreview", ImGuiColorEditFlags_NoSmallPreview);
        tp_ImGuiColorEditFlags->bindEnumValue("NoInputs", ImGuiColorEditFlags_NoInputs);
        tp_ImGuiColorEditFlags->bindEnumValue("NoTooltip", ImGuiColorEditFlags_NoTooltip);
        tp_ImGuiColorEditFlags->bindEnumValue("NoLabel", ImGuiColorEditFlags_NoLabel);
        tp_ImGuiColorEditFlags->bindEnumValue("NoSidePreview", ImGuiColorEditFlags_NoSidePreview);
        tp_ImGuiColorEditFlags->bindEnumValue("NoDragDrop", ImGuiColorEditFlags_NoDragDrop);
        tp_ImGuiColorEditFlags->bindEnumValue("NoBorder", ImGuiColorEditFlags_NoBorder);
        tp_ImGuiColorEditFlags->bindEnumValue("AlphaBar", ImGuiColorEditFlags_AlphaBar);
        tp_ImGuiColorEditFlags->bindEnumValue("AlphaPreview", ImGuiColorEditFlags_AlphaPreview);
        tp_ImGuiColorEditFlags->bindEnumValue("AlphaPreviewHalf", ImGuiColorEditFlags_AlphaPreviewHalf);
        tp_ImGuiColorEditFlags->bindEnumValue("HDR", ImGuiColorEditFlags_HDR);
        tp_ImGuiColorEditFlags->bindEnumValue("DisplayRGB", ImGuiColorEditFlags_DisplayRGB);
        tp_ImGuiColorEditFlags->bindEnumValue("DisplayHSV", ImGuiColorEditFlags_DisplayHSV);
        tp_ImGuiColorEditFlags->bindEnumValue("DisplayHex", ImGuiColorEditFlags_DisplayHex);
        tp_ImGuiColorEditFlags->bindEnumValue("Uint8", ImGuiColorEditFlags_Uint8);
        tp_ImGuiColorEditFlags->bindEnumValue("Float", ImGuiColorEditFlags_Float);
        tp_ImGuiColorEditFlags->bindEnumValue("PickerHueBar", ImGuiColorEditFlags_PickerHueBar);
        tp_ImGuiColorEditFlags->bindEnumValue("PickerHueWheel", ImGuiColorEditFlags_PickerHueWheel);
        tp_ImGuiColorEditFlags->bindEnumValue("InputRGB", ImGuiColorEditFlags_InputRGB);
        tp_ImGuiColorEditFlags->bindEnumValue("InputHSV", ImGuiColorEditFlags_InputHSV);
        tp_ImGuiColorEditFlags->bindEnumValue("DefaultOptions_", ImGuiColorEditFlags_DefaultOptions_);
        tp_ImGuiColorEditFlags->bindEnumValue("DisplayMask_", ImGuiColorEditFlags_DisplayMask_);
        tp_ImGuiColorEditFlags->bindEnumValue("DataTypeMask_", ImGuiColorEditFlags_DataTypeMask_);
        tp_ImGuiColorEditFlags->bindEnumValue("PickerMask_", ImGuiColorEditFlags_PickerMask_);
        tp_ImGuiColorEditFlags->bindEnumValue("InputMask_", ImGuiColorEditFlags_InputMask_);
        
        tp_ImGuiSliderFlags->bindEnumValue("None", ImGuiSliderFlags_None);
        tp_ImGuiSliderFlags->bindEnumValue("AlwaysClamp", ImGuiSliderFlags_AlwaysClamp);
        tp_ImGuiSliderFlags->bindEnumValue("Logarithmic", ImGuiSliderFlags_Logarithmic);
        tp_ImGuiSliderFlags->bindEnumValue("NoRoundToFormat", ImGuiSliderFlags_NoRoundToFormat);
        tp_ImGuiSliderFlags->bindEnumValue("NoInput", ImGuiSliderFlags_NoInput);
        tp_ImGuiSliderFlags->bindEnumValue("InvalidMask_", ImGuiSliderFlags_InvalidMask_);
        
        tp_ImGuiMouseButton->bindEnumValue("Left", ImGuiMouseButton_Left);
        tp_ImGuiMouseButton->bindEnumValue("Right", ImGuiMouseButton_Right);
        tp_ImGuiMouseButton->bindEnumValue("Middle", ImGuiMouseButton_Middle);
        tp_ImGuiMouseCursor->bindEnumValue("None", ImGuiMouseCursor_None);
        tp_ImGuiMouseCursor->bindEnumValue("Arrow", ImGuiMouseCursor_Arrow);
        tp_ImGuiMouseCursor->bindEnumValue("TextInput", ImGuiMouseCursor_TextInput);
        tp_ImGuiMouseCursor->bindEnumValue("ResizeAll", ImGuiMouseCursor_ResizeAll);
        tp_ImGuiMouseCursor->bindEnumValue("ResizeNS", ImGuiMouseCursor_ResizeNS);
        tp_ImGuiMouseCursor->bindEnumValue("ResizeEW", ImGuiMouseCursor_ResizeEW);
        tp_ImGuiMouseCursor->bindEnumValue("ResizeNESW", ImGuiMouseCursor_ResizeNESW);
        tp_ImGuiMouseCursor->bindEnumValue("ResizeNWSE", ImGuiMouseCursor_ResizeNWSE);
        tp_ImGuiMouseCursor->bindEnumValue("Hand", ImGuiMouseCursor_Hand);
        tp_ImGuiMouseCursor->bindEnumValue("NotAllowed", ImGuiMouseCursor_NotAllowed);
        tp_ImGuiMouseSource->bindEnumValue("Mouse", ImGuiMouseSource_Mouse);
        tp_ImGuiMouseSource->bindEnumValue("TouchScreen", ImGuiMouseSource_TouchScreen);
        tp_ImGuiMouseSource->bindEnumValue("Pen", ImGuiMouseSource_Pen);
        
        tp_ImGuiCond->bindEnumValue("None", ImGuiCond_None);
        tp_ImGuiCond->bindEnumValue("Always", ImGuiCond_Always);
        tp_ImGuiCond->bindEnumValue("Once", ImGuiCond_Once);
        tp_ImGuiCond->bindEnumValue("FirstUseEver", ImGuiCond_FirstUseEver);
        tp_ImGuiCond->bindEnumValue("Appearing", ImGuiCond_Appearing);
        
        tp_ImGuiTableFlags->bindEnumValue("None", ImGuiTableFlags_None);
        tp_ImGuiTableFlags->bindEnumValue("Resizable", ImGuiTableFlags_Resizable);
        tp_ImGuiTableFlags->bindEnumValue("Reorderable", ImGuiTableFlags_Reorderable);
        tp_ImGuiTableFlags->bindEnumValue("Hideable", ImGuiTableFlags_Hideable);
        tp_ImGuiTableFlags->bindEnumValue("Sortable", ImGuiTableFlags_Sortable);
        tp_ImGuiTableFlags->bindEnumValue("NoSavedSettings", ImGuiTableFlags_NoSavedSettings);
        tp_ImGuiTableFlags->bindEnumValue("ContextMenuInBody", ImGuiTableFlags_ContextMenuInBody);
        tp_ImGuiTableFlags->bindEnumValue("RowBg", ImGuiTableFlags_RowBg);
        tp_ImGuiTableFlags->bindEnumValue("BordersInnerH", ImGuiTableFlags_BordersInnerH);
        tp_ImGuiTableFlags->bindEnumValue("BordersOuterH", ImGuiTableFlags_BordersOuterH);
        tp_ImGuiTableFlags->bindEnumValue("BordersInnerV", ImGuiTableFlags_BordersInnerV);
        tp_ImGuiTableFlags->bindEnumValue("BordersOuterV", ImGuiTableFlags_BordersOuterV);
        tp_ImGuiTableFlags->bindEnumValue("BordersH", ImGuiTableFlags_BordersH);
        tp_ImGuiTableFlags->bindEnumValue("BordersV", ImGuiTableFlags_BordersV);
        tp_ImGuiTableFlags->bindEnumValue("BordersInner", ImGuiTableFlags_BordersInner);
        tp_ImGuiTableFlags->bindEnumValue("BordersOuter", ImGuiTableFlags_BordersOuter);
        tp_ImGuiTableFlags->bindEnumValue("Borders", ImGuiTableFlags_Borders);
        tp_ImGuiTableFlags->bindEnumValue("NoBordersInBody", ImGuiTableFlags_NoBordersInBody);
        tp_ImGuiTableFlags->bindEnumValue("NoBordersInBodyUntilResize", ImGuiTableFlags_NoBordersInBodyUntilResize);
        tp_ImGuiTableFlags->bindEnumValue("SizingFixedFit", ImGuiTableFlags_SizingFixedFit);
        tp_ImGuiTableFlags->bindEnumValue("SizingFixedSame", ImGuiTableFlags_SizingFixedSame);
        tp_ImGuiTableFlags->bindEnumValue("SizingStretchProp", ImGuiTableFlags_SizingStretchProp);
        tp_ImGuiTableFlags->bindEnumValue("SizingStretchSame", ImGuiTableFlags_SizingStretchSame);
        tp_ImGuiTableFlags->bindEnumValue("NoHostExtendX", ImGuiTableFlags_NoHostExtendX);
        tp_ImGuiTableFlags->bindEnumValue("NoHostExtendY", ImGuiTableFlags_NoHostExtendY);
        tp_ImGuiTableFlags->bindEnumValue("NoKeepColumnsVisible", ImGuiTableFlags_NoKeepColumnsVisible);
        tp_ImGuiTableFlags->bindEnumValue("PreciseWidths", ImGuiTableFlags_PreciseWidths);
        tp_ImGuiTableFlags->bindEnumValue("NoClip", ImGuiTableFlags_NoClip);
        tp_ImGuiTableFlags->bindEnumValue("PadOuterX", ImGuiTableFlags_PadOuterX);
        tp_ImGuiTableFlags->bindEnumValue("NoPadOuterX", ImGuiTableFlags_NoPadOuterX);
        tp_ImGuiTableFlags->bindEnumValue("NoPadInnerX", ImGuiTableFlags_NoPadInnerX);
        tp_ImGuiTableFlags->bindEnumValue("ScrollX", ImGuiTableFlags_ScrollX);
        tp_ImGuiTableFlags->bindEnumValue("ScrollY", ImGuiTableFlags_ScrollY);
        tp_ImGuiTableFlags->bindEnumValue("SortMulti", ImGuiTableFlags_SortMulti);
        tp_ImGuiTableFlags->bindEnumValue("SortTristate", ImGuiTableFlags_SortTristate);
        tp_ImGuiTableFlags->bindEnumValue("HighlightHoveredColumn", ImGuiTableFlags_HighlightHoveredColumn);
        tp_ImGuiTableFlags->bindEnumValue("SizingMask_", ImGuiTableFlags_SizingMask_);
        
        tp_ImGuiTableColumnFlags->bindEnumValue("None", ImGuiTableColumnFlags_None);
        tp_ImGuiTableColumnFlags->bindEnumValue("Disabled", ImGuiTableColumnFlags_Disabled);
        tp_ImGuiTableColumnFlags->bindEnumValue("DefaultHide", ImGuiTableColumnFlags_DefaultHide);
        tp_ImGuiTableColumnFlags->bindEnumValue("DefaultSort", ImGuiTableColumnFlags_DefaultSort);
        tp_ImGuiTableColumnFlags->bindEnumValue("WidthStretch", ImGuiTableColumnFlags_WidthStretch);
        tp_ImGuiTableColumnFlags->bindEnumValue("WidthFixed", ImGuiTableColumnFlags_WidthFixed);
        tp_ImGuiTableColumnFlags->bindEnumValue("NoResize", ImGuiTableColumnFlags_NoResize);
        tp_ImGuiTableColumnFlags->bindEnumValue("NoReorder", ImGuiTableColumnFlags_NoReorder);
        tp_ImGuiTableColumnFlags->bindEnumValue("NoHide", ImGuiTableColumnFlags_NoHide);
        tp_ImGuiTableColumnFlags->bindEnumValue("NoClip", ImGuiTableColumnFlags_NoClip);
        tp_ImGuiTableColumnFlags->bindEnumValue("NoSort", ImGuiTableColumnFlags_NoSort);
        tp_ImGuiTableColumnFlags->bindEnumValue("NoSortAscending", ImGuiTableColumnFlags_NoSortAscending);
        tp_ImGuiTableColumnFlags->bindEnumValue("NoSortDescending", ImGuiTableColumnFlags_NoSortDescending);
        tp_ImGuiTableColumnFlags->bindEnumValue("NoHeaderLabel", ImGuiTableColumnFlags_NoHeaderLabel);
        tp_ImGuiTableColumnFlags->bindEnumValue("NoHeaderWidth", ImGuiTableColumnFlags_NoHeaderWidth);
        tp_ImGuiTableColumnFlags->bindEnumValue("PreferSortAscending", ImGuiTableColumnFlags_PreferSortAscending);
        tp_ImGuiTableColumnFlags->bindEnumValue("PreferSortDescending", ImGuiTableColumnFlags_PreferSortDescending);
        tp_ImGuiTableColumnFlags->bindEnumValue("IndentEnable", ImGuiTableColumnFlags_IndentEnable);
        tp_ImGuiTableColumnFlags->bindEnumValue("IndentDisable", ImGuiTableColumnFlags_IndentDisable);
        tp_ImGuiTableColumnFlags->bindEnumValue("AngledHeader", ImGuiTableColumnFlags_AngledHeader);
        tp_ImGuiTableColumnFlags->bindEnumValue("IsEnabled", ImGuiTableColumnFlags_IsEnabled);
        tp_ImGuiTableColumnFlags->bindEnumValue("IsVisible", ImGuiTableColumnFlags_IsVisible);
        tp_ImGuiTableColumnFlags->bindEnumValue("IsSorted", ImGuiTableColumnFlags_IsSorted);
        tp_ImGuiTableColumnFlags->bindEnumValue("IsHovered", ImGuiTableColumnFlags_IsHovered);
        tp_ImGuiTableColumnFlags->bindEnumValue("WidthMask_", ImGuiTableColumnFlags_WidthMask_);
        tp_ImGuiTableColumnFlags->bindEnumValue("IndentMask_", ImGuiTableColumnFlags_IndentMask_);
        tp_ImGuiTableColumnFlags->bindEnumValue("StatusMask_", ImGuiTableColumnFlags_StatusMask_);
        tp_ImGuiTableColumnFlags->bindEnumValue("NoDirectResize_", ImGuiTableColumnFlags_NoDirectResize_);
        
        tp_ImGuiTableRowFlags->bindEnumValue("None", ImGuiTableRowFlags_None);
        tp_ImGuiTableRowFlags->bindEnumValue("Headers", ImGuiTableRowFlags_Headers);

        tp_ImGuiTableBgTarget->bindEnumValue("None", ImGuiTableBgTarget_None);
        tp_ImGuiTableBgTarget->bindEnumValue("RowBg0", ImGuiTableBgTarget_RowBg0);
        tp_ImGuiTableBgTarget->bindEnumValue("RowBg1", ImGuiTableBgTarget_RowBg1);
        tp_ImGuiTableBgTarget->bindEnumValue("CellBg", ImGuiTableBgTarget_CellBg);
    }
    void BindImGui(IScriptAPI* api) {
        BindImGuiEnums(api);

        DataType* tp_ImDrawChannel = api->bind<ImDrawChannel>("ImDrawChannel");
        DataType* tp_ImDrawCmd = api->bind<ImDrawCmd>("ImDrawCmd");
        DataType* tp_ImDrawData = api->bind<ImDrawData>("ImDrawData");
        DataType* tp_ImDrawList = api->bind<ImDrawList>("ImDrawList");
        DataType* tp_ImDrawListSharedData = api->bind<ImDrawListSharedData>("ImDrawListSharedData");
        DataType* tp_ImDrawListSplitter = api->bind<ImDrawListSplitter>("ImDrawListSplitter");
        DataType* tp_ImDrawVert = api->bind<ImDrawVert>("ImDrawVert");
        DataType* tp_ImFont = api->bind<ImFont>("ImFont");
        DataType* tp_ImFontAtlas = api->bind<ImFontAtlas>("ImFontAtlas");
        DataType* tp_ImFontBuilderIO = api->bind<ImFontBuilderIO>("ImFontBuilderIO");
        DataType* tp_ImFontConfig = api->bind<ImFontConfig>("ImFontConfig");
        DataType* tp_ImFontGlyph = api->bind<ImFontGlyph>("ImFontGlyph");
        DataType* tp_ImFontGlyphRangesBuilder = api->bind<ImFontGlyphRangesBuilder>("ImFontGlyphRangesBuilder");
        DataType* tp_ImColor = api->bind<ImColor>("ImColor");
        DataType* tp_ImGuiContext = api->bind<ImGuiContext>("ImGuiContext");
        DataType* tp_ImGuiIO = api->bind<ImGuiIO>("ImGuiIO");
        DataType* tp_ImGuiInputTextCallbackData = api->bind<ImGuiInputTextCallbackData>("ImGuiInputTextCallbackData");
        DataType* tp_ImGuiKeyData = api->bind<ImGuiKeyData>("ImGuiKeyData");
        DataType* tp_ImGuiListClipper = api->bind<ImGuiListClipper>("ImGuiListClipper");
        DataType* tp_ImGuiOnceUponAFrame = api->bind<ImGuiOnceUponAFrame>("ImGuiOnceUponAFrame");
        DataType* tp_ImGuiPayload = api->bind<ImGuiPayload>("ImGuiPayload");
        DataType* tp_ImGuiPlatformImeData = api->bind<ImGuiPlatformImeData>("ImGuiPlatformImeData");
        DataType* tp_ImGuiSizeCallbackData = api->bind<ImGuiSizeCallbackData>("ImGuiSizeCallbackData");
        DataType* tp_ImGuiStorage = api->bind<ImGuiStorage>("ImGuiStorage");
        DataType* tp_ImGuiStyle = api->bind<ImGuiStyle>("ImGuiStyle");
        DataType* tp_ImGuiTableSortSpecs = api->bind<ImGuiTableSortSpecs>("ImGuiTableSortSpecs");
        DataType* tp_ImGuiTableColumnSortSpecs = api->bind<ImGuiTableColumnSortSpecs>("ImGuiTableColumnSortSpecs");
        DataType* tp_ImGuiTextBuffer = api->bind<ImGuiTextBuffer>("ImGuiTextBuffer");
        DataType* tp_ImGuiTextFilter = api->bind<ImGuiTextFilter>("ImGuiTextFilter");
        DataType* tp_ImGuiViewport = api->bind<ImGuiViewport>("ImGuiViewport");

        DataType* tp_ImTextureID = api->bind<ImTextureID>("ImTextureID");
        DataType* tp_ImDrawIdx = api->bind<ImDrawIdx>("ImDrawIdx");

        DataType* tp_ImVec2 = api->bind<ImVec2>("ImVec2");
        tp_ImVec2->bind("x", &ImVec2::x);
        tp_ImVec2->bind("y", &ImVec2::y);
        tp_ImVec2->setFlags(DataType::Flags::HostConstructionNotRequired);

        DataType* tp_ImVec4 = api->bind<ImVec4>("ImVec4");
        tp_ImVec4->bind("x", &ImVec4::x);
        tp_ImVec4->bind("y", &ImVec4::y);
        tp_ImVec4->bind("z", &ImVec4::z);
        tp_ImVec4->bind("w", &ImVec4::w);
        tp_ImVec4->setFlags(DataType::Flags::HostConstructionNotRequired);

        api->bind("GetCurrentContext", &ImGui::GetCurrentContext);
        api->bind("SetCurrentContext", &ImGui::SetCurrentContext)->setArgNames({ "ctx" });
        api->bind("GetIO", &ImGui::GetIO);
        api->bind("GetStyle", &ImGui::GetStyle);
        api->bind("ShowDemoWindow", +[](bool isOpen){ ImGui::ShowDemoWindow(&isOpen); return isOpen; })->setArgNames({ "isOpen" });
        api->bind("ShowMetricsWindow", +[](bool isOpen){ ImGui::ShowMetricsWindow(&isOpen); return isOpen; })->setArgNames({ "isOpen" });
        api->bind("ShowDebugLogWindow", +[](bool isOpen){ ImGui::ShowDebugLogWindow(&isOpen); return isOpen; })->setArgNames({ "isOpen" });
        api->bind("ShowIDStackToolWindow", +[](bool isOpen){ ImGui::ShowIDStackToolWindow(&isOpen); return isOpen; })->setArgNames({ "isOpen" });
        api->bind("ShowAboutWindow", +[](bool isOpen){ ImGui::ShowIDStackToolWindow(&isOpen); return isOpen; })->setArgNames({ "isOpen" });
        api->bind("ShowStyleEditor", &ImGui::ShowStyleEditor)->setArgNames({ "targetStyle" });
        api->bind("ShowStyleSelector", &ImGui::ShowStyleSelector)->setArgNames({ "label" });
        api->bind("ShowFontSelector", &ImGui::ShowFontSelector)->setArgNames({ "label" });;
        api->bind("ShowUserGuide", &ImGui::ShowUserGuide);
        api->bind("GetVersion", &ImGui::GetVersion);
        api->bind("StyleColorsDark", &ImGui::StyleColorsDark)->setArgNames({ "targetStyle" });
        api->bind("StyleColorsLight", &ImGui::StyleColorsLight)->setArgNames({ "targetStyle" });
        api->bind("StyleColorsClassic", &ImGui::StyleColorsClassic)->setArgNames({ "targetStyle" });
        api->bind("Begin", +[](const char* title, ImGuiWindowFlags_ flags) { return ImGui::Begin(title, nullptr, flags); })->setArgNames({ "title", "flags" });
        api->bind("End", &ImGui::End);
        api->bind("BeginChild", +[](ImGuiID id, const ImVec2& size, ImGuiChildFlags_ childFlags, ImGuiWindowFlags_ windowFlags) { return ImGui::BeginChild(id, size, childFlags, windowFlags); })->setArgNames({ "id", "size", "childFlags", "windowFlags" });
        api->bind("EndChild", &ImGui::EndChild);
        api->bind("IsWindowAppearing", &ImGui::IsWindowAppearing);
        api->bind("IsWindowCollapsed", &ImGui::IsWindowCollapsed);
        api->bind("IsWindowFocused", +[](ImGuiFocusedFlags_ flags) { return ImGui::IsWindowFocused(flags); })->setArgNames({ "flags" });
        api->bind("IsWindowHovered", &ImGui::IsWindowHovered);
        api->bind("GetWindowDrawList", &ImGui::GetWindowDrawList);
        api->bind("GetWindowPos", &ImGui::GetWindowPos);
        api->bind("GetWindowSize", &ImGui::GetWindowSize);
        api->bind("GetWindowWidth", &ImGui::GetWindowWidth);
        api->bind("GetWindowHeight", &ImGui::GetWindowHeight);
        api->bind("SetNextWindowPos", +[](const ImVec2& pos) { ImGui::SetNextWindowPos(pos); })->setArgNames({ "pos" });
        api->bind("SetNextWindowSize", +[](const ImVec2& size) { ImGui::SetNextWindowSize(size); })->setArgNames({ "size" });
        api->bind("SetNextWindowSizeConstraints", +[](const ImVec2& min, const ImVec2& max) { ImGui::SetNextWindowSizeConstraints(min, max); })->setArgNames({ "minSize", "maxSize" });
        api->bind("SetNextWindowContentSize", &ImGui::SetNextWindowContentSize)->setArgNames({ "size" });
        api->bind("SetNextWindowCollapsed", +[](bool isCollapsed) { ImGui::SetNextWindowCollapsed(isCollapsed); })->setArgNames({ "isCollapsed" });
        api->bind("SetNextWindowFocus", &ImGui::SetNextWindowFocus);
        api->bind("SetNextWindowScroll", &ImGui::SetNextWindowScroll)->setArgNames({ "scrollValue" });
        api->bind("SetNextWindowBgAlpha", &ImGui::SetNextWindowBgAlpha)->setArgNames({ "alpha" });
        api->bind("SetWindowPos", +[](const ImVec2& pos) { ImGui::SetWindowPos(pos); })->setArgNames({ "position" });
        api->bind("SetWindowSize", +[](const ImVec2& size) { ImGui::SetWindowSize(size); })->setArgNames({ "size" });
        api->bind("SetWindowCollapsed", +[](bool isCollapsed) { ImGui::SetWindowCollapsed(isCollapsed); })->setArgNames({ "isCollapsed" });
        api->bind("SetWindowFocus", +[]() { ImGui::SetWindowFocus(); });
        api->bind("SetWindowFontScale", &ImGui::SetWindowFontScale)->setArgNames({ "fontScale" });
        api->bind("GetContentRegionAvail", &ImGui::GetContentRegionAvail);
        api->bind("GetContentRegionMax", &ImGui::GetContentRegionMax);
        api->bind("GetWindowContentRegionMin", &ImGui::GetWindowContentRegionMin);
        api->bind("GetWindowContentRegionMax", &ImGui::GetWindowContentRegionMax);

        api->bind("GetScrollX", &ImGui::GetScrollX);
        api->bind("GetScrollY", &ImGui::GetScrollY);
        api->bind("SetScrollX", &ImGui::SetScrollX)->setArgNames({ "scrollX" });
        api->bind("SetScrollY", &ImGui::SetScrollY)->setArgNames({ "scrollY" });
        api->bind("GetScrollMaxX", &ImGui::GetScrollMaxX);
        api->bind("GetScrollMaxY", &ImGui::GetScrollMaxY);
        api->bind("SetScrollHereX", &ImGui::SetScrollHereX)->setArgNames({ "centerRatioX" });
        api->bind("SetScrollHereY", &ImGui::SetScrollHereY)->setArgNames({ "centerRatioY" });
        api->bind("SetScrollFromPosX", &ImGui::SetScrollFromPosX)->setArgNames({ "localX", "centerRatioX" });
        api->bind("SetScrollFromPosY", &ImGui::SetScrollFromPosY)->setArgNames({ "localY", "centerRatioY" });
        api->bind("PushFont", &ImGui::PushFont)->setArgNames({ "font" });
        api->bind("PopFont", &ImGui::PopFont);
        api->bind("PushStyleColor", +[](ImGuiCol_ idx, const ImVec4& col) { ImGui::PushStyleColor(idx, col); })->setArgNames({ "index", "color" });
        api->bind("PopStyleColor", +[]() { ImGui::PopStyleColor(); });
        api->bind("PushStyleVarF32", +[](ImGuiStyleVar_ idx, f32 val) { ImGui::PushStyleVar(idx, val); })->setArgNames({ "index", "value" });
        api->bind("PushStyleVarVec2", +[](ImGuiStyleVar_ idx, const ImVec2& val) { ImGui::PushStyleVar(idx, val); })->setArgNames({ "index", "value" });
        api->bind("PopStyleVar", +[]() { ImGui::PopStyleVar(); });
        api->bind("PushTabStop", &ImGui::PushTabStop)->setArgNames({ "tabStop" });
        api->bind("PopTabStop", &ImGui::PopTabStop);
        api->bind("PushButtonRepeat", &ImGui::PushButtonRepeat)->setArgNames({ "repeat" });
        api->bind("PopButtonRepeat", &ImGui::PopButtonRepeat);
        api->bind("PushItemWidth", &ImGui::PushItemWidth)->setArgNames({ "itemWidth" });
        api->bind("PopItemWidth", &ImGui::PopItemWidth);
        api->bind("SetNextItemWidth", &ImGui::SetNextItemWidth)->setArgNames({ "itemWidth" });
        api->bind("CalcItemWidth", &ImGui::CalcItemWidth);
        api->bind("PushTextWrapPos", &ImGui::PushTextWrapPos)->setArgNames({ "wrapLocalPosX" });
        api->bind("PopTextWrapPos", &ImGui::PopTextWrapPos);
        api->bind("GetFont", &ImGui::GetFont);
        api->bind("GetFontSize", &ImGui::GetFontSize);
        api->bind("GetFontTexUvWhitePixel", &ImGui::GetFontTexUvWhitePixel);
        api->bind("GetStyleColorVec4", +[](ImGuiCol_ idx) { return ImGui::GetStyleColorVec4(idx); })->setArgNames({ "index" });
        api->bind("GetCursorScreenPos", &ImGui::GetCursorScreenPos);
        api->bind("SetCursorScreenPos", &ImGui::SetCursorScreenPos)->setArgNames({ "position" });
        api->bind("GetCursorPos", &ImGui::GetCursorPos);
        api->bind("GetCursorPosX", &ImGui::GetCursorPosX);
        api->bind("GetCursorPosY", &ImGui::GetCursorPosY);
        api->bind("SetCursorPos", &ImGui::SetCursorPos)->setArgNames({ "localPosition" });
        api->bind("SetCursorPosX", &ImGui::SetCursorPosX)->setArgNames({ "localX" });
        api->bind("SetCursorPosY", &ImGui::SetCursorPosY)->setArgNames({ "localY" });
        api->bind("GetCursorStartPos", &ImGui::GetCursorStartPos);

        api->bind("Separator", &ImGui::Separator);
        api->bind("SameLine", +[](f32 offset, f32 spacing) { ImGui::SameLine(offset, spacing); })->setArgNames({ "offsetX", "spacingX" });
        api->bind("NewLine", &ImGui::NewLine);
        api->bind("Spacing", &ImGui::Spacing);
        api->bind("Dummy", &ImGui::Dummy)->setArgNames({ "size" });
        api->bind("Indent", &ImGui::Indent)->setArgNames({ "indentWidth" });
        api->bind("Unindent", &ImGui::Unindent)->setArgNames({ "indentWidth" });
        api->bind("BeginGroup", &ImGui::BeginGroup);
        api->bind("EndGroup", &ImGui::EndGroup);
        api->bind("AlignTextToFramePadding", &ImGui::AlignTextToFramePadding);
        api->bind("GetTextLineHeight", &ImGui::GetTextLineHeight);
        api->bind("GetTextLineHeightWithSpacing", &ImGui::GetTextLineHeightWithSpacing);
        api->bind("GetFrameHeight", &ImGui::GetFrameHeight);
        api->bind("GetFrameHeightWithSpacing", &ImGui::GetFrameHeightWithSpacing);
        api->bind<void, int>("PushID", &ImGui::PushID)->setArgNames({ "id" });
        api->bind("PopID", &ImGui::PopID);
        api->bind<ImGuiID, const char*>("GetID", &ImGui::GetID)->setArgNames({ "id" });
        api->bind("Text", +[](const char* text) { ImGui::TextUnformatted(text); })->setArgNames({ "text" });
        api->bind("TextColored", +[](const ImVec4& color, const char* text) { ImGui::TextColored(color, text); })->setArgNames({ "color", "text" });
        api->bind("TextDisabled", +[](const char* text) { ImGui::TextDisabled(text); })->setArgNames({ "text" });
        api->bind("TextWrapped", +[](const char* text) { ImGui::TextWrapped(text); })->setArgNames({ "text" });
        api->bind("LabelText", +[](const char* label, const char* text) { ImGui::LabelText(label, text); })->setArgNames({ "label", "text" });
        api->bind("BulletText", +[](const char* text) { ImGui::BulletText(text); })->setArgNames({ "text" });
        api->bind("SeparatorText", &ImGui::SeparatorText)->setArgNames({ "label" });
        api->bind("Button", &ImGui::Button)->setArgNames({ "label", "size" });
        api->bind("SmallButton", &ImGui::SmallButton)->setArgNames({ "label" });
        api->bind("InvisibleButton", +[](const char* id, const ImVec2& size, ImGuiButtonFlags_ flags) { return ImGui::InvisibleButton(id, size, flags); })->setArgNames({ "id", "size", "flags" });
        api->bind("ArrowButton", +[](const char* id, ImGuiDir_ dir) { return ImGui::ArrowButton(id, dir); })->setArgNames({ "id", "direction" });
        api->bind("Checkbox", +[](const char* label, bool value) { return ImGui::Checkbox(label, &value); })->setArgNames({ "label", "value" });
        api->bind<bool, const char*, bool>("RadioButton", &ImGui::RadioButton)->setArgNames({ "label", "active" });
        api->bind("ProgressBar", +[](f32 frac) { ImGui::ProgressBar(frac); })->setArgNames({ "fraction" });
        api->bind("Bullet", &ImGui::Bullet);
        api->bind("Image", +[](ImTextureID texId, const ImVec2& size) { ImGui::Image(texId, size); })->setArgNames({ "textureId", "size" });
        api->bind("ImageButton", +[](ImTextureID texId, const ImVec2& size) { return ImGui::ImageButton(texId, size); })->setArgNames({ "textureId", "size" });
        
        // todo: array arguments
        // api->bind("BeginCombo", +[](const char* label, const char* previewVal, ImGuiComboFlags_ flags) { return ImGui::BeginCombo(label, previewVal, flags); })->setArgNames({ "label", "previewValue", "flags" });
        // api->bind("EndCombo", &ImGui::EndCombo);
        // api->bind("Combo", &ImGui::Combo);

        api->bind("DragFloat", +[](const char* label, f32 value, Callback<void, f32>* onChange) {
            bool didChange = ImGui::DragFloat(label, &value);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("DragFloat2", +[](const char* label, utils::vec2f& value, Callback<void, const utils::vec2f&>* onChange) {
            bool didChange = ImGui::DragFloat2(label, &value.x);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("DragFloat3", +[](const char* label, utils::vec3f& value, Callback<void, const utils::vec3f&>* onChange) {
            bool didChange = ImGui::DragFloat3(label, &value.x);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("DragFloat4", +[](const char* label, utils::vec4f& value, Callback<void, const utils::vec4f&>* onChange) {
            bool didChange = ImGui::DragFloat4(label, &value.x);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("DragInt", +[](const char* label, i32 value, Callback<void, i32>* onChange) {
            bool didChange = ImGui::DragInt(label, &value);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("DragInt2", +[](const char* label, utils::vec2i& value, Callback<void, utils::vec2i&>* onChange) {
            bool didChange = ImGui::DragInt2(label, &value.x);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("DragInt3", +[](const char* label, utils::vec3i& value, Callback<void, utils::vec3i&>* onChange) {
            bool didChange = ImGui::DragInt3(label, &value.x);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("DragInt4", +[](const char* label, utils::vec4i& value, Callback<void, utils::vec4i&>* onChange) {
            bool didChange = ImGui::DragInt4(label, &value.x);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("SliderFloat", +[](const char* label, f32 value, f32 min, f32 max, Callback<void, f32>* onChange) {
            bool didChange = ImGui::SliderFloat(label, &value, min, max);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "min", "max", "onChange" });
        api->bind("SliderFloat2", +[](const char* label, utils::vec2f& value, f32 min, f32 max, Callback<void, utils::vec2f&>* onChange) {
            bool didChange = ImGui::SliderFloat2(label, &value.x, min, max);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "min", "max", "onChange" });
        api->bind("SliderFloat3", +[](const char* label, utils::vec3f& value, f32 min, f32 max, Callback<void, utils::vec3f&>* onChange) {
            bool didChange = ImGui::SliderFloat3(label, &value.x, min, max);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "min", "max", "onChange" });
        api->bind("SliderFloat4", +[](const char* label, utils::vec4f& value, f32 min, f32 max, Callback<void, utils::vec4f&>* onChange) {
            bool didChange = ImGui::SliderFloat4(label, &value.x, min, max);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "min", "max", "onChange" });
        api->bind("SliderAngle", +[](const char* label, f32 value, f32 min, f32 max, Callback<void, f32>* onChange) {
            bool didChange = ImGui::SliderAngle(label, &value, min, max);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "min", "max", "onChange" });
        api->bind("SliderInt", +[](const char* label, i32 value, i32 min, i32 max, Callback<void, f32>* onChange) {
            bool didChange = ImGui::SliderInt(label, &value, min, max);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "min", "max", "onChange" });
        api->bind("SliderInt2", +[](const char* label, utils::vec2i& value, i32 min, i32 max, Callback<void, utils::vec2i&>* onChange) {
            bool didChange = ImGui::SliderInt2(label, &value.x, min, max);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "min", "max", "onChange" });
        api->bind("SliderInt3", +[](const char* label, utils::vec3i& value, i32 min, i32 max, Callback<void, utils::vec3i&>* onChange) {
            bool didChange = ImGui::SliderInt3(label, &value.x, min, max);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "min", "max", "onChange" });
        api->bind("SliderInt4", +[](const char* label, utils::vec4i& value, i32 min, i32 max, Callback<void, utils::vec4i&>* onChange) {
            bool didChange = ImGui::SliderInt4(label, &value.x, min, max);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "min", "max", "onChange" });
        api->bind("VSliderFloat", +[](const char* label, const ImVec2& size, f32 value, f32 min, f32 max, Callback<void, f32>* onChange) {
            bool didChange = ImGui::VSliderFloat(label, size, &value, min, max);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "size", "value", "min", "max", "onChange" });
        api->bind("VSliderInt", +[](const char* label, const ImVec2& size, i32 value, i32 min, i32 max, Callback<void, i32>* onChange) {
            bool didChange = ImGui::VSliderInt(label, size, &value, min, max);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "size", "value", "min", "max", "onChange" });
        api->bind("InputText", +[](const char* label, const char* value, Callback<void, const char*>* onChange) {
            char buf[512] = { 0 };
            strncpy(buf, value, 512);
            bool didChange = ImGui::InputText(label, buf, 512);
            if (onChange) {
                if (didChange) onChange->call(buf);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("InputTextMultiline", +[](const char* label, const char* value, const ImVec2& size, Callback<void, const char*>* onChange) {
            char buf[1024] = { 0 };
            strncpy(buf, value, 1024);
            bool didChange = ImGui::InputTextMultiline(label, buf, 1024, size);
            if (onChange) {
                if (didChange) onChange->call(buf);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "size", "onChange" });
        api->bind("InputTextWithHint", +[](const char* label, const char* value, const char* hint, Callback<void, const char*>* onChange) {
            char buf[512] = { 0 };
            strncpy(buf, value, 512);
            bool didChange = ImGui::InputTextWithHint(label, hint, buf, 512);
            if (onChange) {
                if (didChange) onChange->call(buf);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("InputFloat", +[](const char* label, f32 value, Callback<void, f32>* onChange) {
            bool didChange = ImGui::InputFloat(label, &value);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("InputFloat2", +[](const char* label, utils::vec2f& value, Callback<void, const utils::vec2f&>* onChange) {
            bool didChange = ImGui::InputFloat2(label, &value.x);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("InputFloat3", +[](const char* label, utils::vec3f& value, Callback<void, const utils::vec3f&>* onChange) {
            bool didChange = ImGui::InputFloat3(label, &value.x);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("InputFloat4", +[](const char* label, utils::vec4f& value, Callback<void, const utils::vec4f&>* onChange) {
            bool didChange = ImGui::InputFloat4(label, &value.x);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("InputInt", +[](const char* label, i32 value, Callback<void, i32>* onChange) {
            bool didChange = ImGui::InputInt(label, &value);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("InputInt2", +[](const char* label, utils::vec2i& value, Callback<void, utils::vec2i&>* onChange) {
            bool didChange = ImGui::InputInt2(label, &value.x);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("InputInt3", +[](const char* label, utils::vec3i& value, Callback<void, utils::vec3i&>* onChange) {
            bool didChange = ImGui::InputInt3(label, &value.x);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("InputInt4", +[](const char* label, utils::vec4i& value, Callback<void, utils::vec4i&>* onChange) {
            bool didChange = ImGui::InputInt4(label, &value.x);
            if (onChange) {
                if (didChange) onChange->call(value);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("ColorEdit3", +[](const char* label, utils::vec3f& color, Callback<void, utils::vec3f&>* onChange) {
            bool didChange = ImGui::ColorEdit3(label, &color.x);
            if (onChange) {
                if (didChange) onChange->call(color);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("ColorEdit4", +[](const char* label, utils::vec4f& color, Callback<void, utils::vec4f&>* onChange) {
            bool didChange = ImGui::ColorEdit4(label, &color.x);
            if (onChange) {
                if (didChange) onChange->call(color);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("ColorPicker3", +[](const char* label, utils::vec3f& color, Callback<void, utils::vec3f&>* onChange) {
            bool didChange = ImGui::ColorPicker3(label, &color.x);
            if (onChange) {
                if (didChange) onChange->call(color);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("ColorPicker4", +[](const char* label, utils::vec4f& color, Callback<void, utils::vec4f&>* onChange) {
            bool didChange = ImGui::ColorPicker4(label, &color.x);
            if (onChange) {
                if (didChange) onChange->call(color);

                // todo: handle callback allocation/deallocation more elegantly...
                delete onChange;
            }
            return didChange;
        })->setArgNames({ "label", "value", "onChange" });
        api->bind("SetColorEditOptions", +[](ImGuiColorEditFlags_ flags) {
            ImGui::SetColorEditOptions(flags);
        })->setArgNames({ "flags" });

        api->bind("TreeNode", &ImGui::TreeNode)->setArgNames({ "label" });
        api->bind("TreeNodeEx", +[](const char* label, ImGuiTreeNodeFlags_ flags) { return ImGui::TreeNodeEx(label, flags); })->setArgNames({ "label", "flags" });
        api->bind("TreePush", +[](const char* id) { ImGui::TreePush(id); })->setArgNames({ "id" });
        api->bind("TreePop", &ImGui::TreePop);
        api->bind("GetTreeNodeToLabelSpacing", &ImGui::GetTreeNodeToLabelSpacing);
        api->bind("CollapsingHeader", +[](const char* label, ImGuiTreeNodeFlags_ flags) { return ImGui::CollapsingHeader(label, flags); })->setArgNames({ "label", "flags" });
        api->bind("SetNextItemOpen", +[](bool isOpen) { return ImGui::SetNextItemOpen(isOpen); })->setArgNames({ "isOpen" });
        api->bind("Selectable", +[](const char* label, bool selected) { return ImGui::Selectable(label, selected); })->setArgNames({ "label", "isSelected" });

        // todo: array arguments
        // api->bind("BeginListBox", &ImGui::BeginListBox);
        // api->bind("EndListBox", &ImGui::EndListBox);
        // api->bind("ListBox", &ImGui::ListBox);
        // api->bind("ListBox", &ImGui::ListBox);
        // api->bind("PlotLines", &ImGui::PlotLines);
        // api->bind("PlotLines", &ImGui::PlotLines);
        // api->bind("PlotHistogram", &ImGui::PlotHistogram);
        // api->bind("PlotHistogram", &ImGui::PlotHistogram);

        api->bind("BeginMenuBar", &ImGui::BeginMenuBar);
        api->bind("EndMenuBar", &ImGui::EndMenuBar);
        api->bind("BeginMainMenuBar", &ImGui::BeginMainMenuBar);
        api->bind("EndMainMenuBar", &ImGui::EndMainMenuBar);
        api->bind("BeginMenu", &ImGui::BeginMenu)->setArgNames({ "label", "isEnabled" });
        api->bind("EndMenu", &ImGui::EndMenu);
        api->bind("MenuItem", +[](const char* label, const char* shortcut, bool selected, bool enabled) {
            return ImGui::MenuItem(label, shortcut, &selected, enabled);
        })->setArgNames({ "label", "shortcut", "isSelected", "isEnabled" })->setArgNullable(1);
        api->bind("BeginTooltip", &ImGui::BeginTooltip);
        api->bind("EndTooltip", &ImGui::EndTooltip);
        api->bind("SetTooltip", +[](const char* tooltip) { ImGui::SetTooltip(tooltip); })->setArgNames({ "tooltip" });
        api->bind("BeginItemTooltip", &ImGui::BeginItemTooltip);
        api->bind("SetItemTooltip", +[](const char* tooltip) { ImGui::SetItemTooltip(tooltip); })->setArgNames({ "tooltip" });
        api->bind("BeginPopup", +[](const char* id, ImGuiWindowFlags_ flags) { return ImGui::BeginPopup(id, flags); })->setArgNames({ "id", "flags" });
        api->bind("BeginPopupModal", +[](const char* id, bool isOpen, ImGuiWindowFlags_ flags) { return ImGui::BeginPopupModal(id, &isOpen, flags); })->setArgNames({ "id", "isOpen", "flags" });
        api->bind("EndPopup", &ImGui::EndPopup);
        api->bind("OpenPopup", +[](const char* id) { ImGui::OpenPopup(id); })->setArgNames({ "popupId" });
        // api->bind("OpenPopupOnItemClick", &ImGui::OpenPopupOnItemClick);
        // api->bind("CloseCurrentPopup", &ImGui::CloseCurrentPopup);
        // api->bind("BeginPopupContextItem", &ImGui::BeginPopupContextItem);
        // api->bind("BeginPopupContextWindow", &ImGui::BeginPopupContextWindow);
        // api->bind("BeginPopupContextVoid", &ImGui::BeginPopupContextVoid);
        // api->bind("IsPopupOpen", &ImGui::IsPopupOpen);
        // api->bind("BeginTable", &ImGui::BeginTable);
        // api->bind("EndTable", &ImGui::EndTable);
        // api->bind("TableNextRow", &ImGui::TableNextRow);
        // api->bind("TableNextColumn", &ImGui::TableNextColumn);
        // api->bind("TableSetColumnIndex", &ImGui::TableSetColumnIndex);
        // api->bind("TableSetupColumn", &ImGui::TableSetupColumn);
        // api->bind("TableSetupScrollFreeze", &ImGui::TableSetupScrollFreeze);
        // api->bind("TableHeader", &ImGui::TableHeader);
        // api->bind("TableHeadersRow", &ImGui::TableHeadersRow);
        // api->bind("TableAngledHeadersRow", &ImGui::TableAngledHeadersRow);
        // api->bind("TableGetSortSpecs", &ImGui::TableGetSortSpecs);
        // api->bind("TableGetColumnCount", &ImGui::TableGetColumnCount);
        // api->bind("TableGetColumnIndex", &ImGui::TableGetColumnIndex);
        // api->bind("TableGetRowIndex", &ImGui::TableGetRowIndex);
        // api->bind("TableGetColumnName", &ImGui::TableGetColumnName);
        // api->bind("TableGetColumnFlags", &ImGui::TableGetColumnFlags);
        // api->bind("TableSetColumnEnabled", &ImGui::TableSetColumnEnabled);
        // api->bind("TableSetBgColor", &ImGui::TableSetBgColor);
        // api->bind("Columns", &ImGui::Columns);
        // api->bind("NextColumn", &ImGui::NextColumn);
        // api->bind("GetColumnIndex", &ImGui::GetColumnIndex);
        // api->bind("GetColumnWidth", &ImGui::GetColumnWidth);
        // api->bind("SetColumnWidth", &ImGui::SetColumnWidth);
        // api->bind("GetColumnOffset", &ImGui::GetColumnOffset);
        // api->bind("SetColumnOffset", &ImGui::SetColumnOffset);
        // api->bind("GetColumnsCount", &ImGui::GetColumnsCount);
        // api->bind("BeginTabBar", &ImGui::BeginTabBar);
        // api->bind("EndTabBar", &ImGui::EndTabBar);
        // api->bind("BeginTabItem", &ImGui::BeginTabItem);
        // api->bind("EndTabItem", &ImGui::EndTabItem);
        // api->bind("TabItemButton", &ImGui::TabItemButton);
        // api->bind("SetTabItemClosed", &ImGui::SetTabItemClosed);
        // api->bind("BeginDragDropSource", &ImGui::BeginDragDropSource);
        // api->bind("SetDragDropPayload", &ImGui::SetDragDropPayload);
        // api->bind("EndDragDropSource", &ImGui::EndDragDropSource);
        // api->bind("BeginDragDropTarget", &ImGui::BeginDragDropTarget);
        // api->bind("AcceptDragDropPayload", &ImGui::AcceptDragDropPayload);
        // api->bind("EndDragDropTarget", &ImGui::EndDragDropTarget);
        // api->bind("GetDragDropPayload", &ImGui::GetDragDropPayload);
        api->bind("BeginDisabled", &ImGui::BeginDisabled)->setArgNames({ "isDisabled" });
        api->bind("EndDisabled", &ImGui::EndDisabled);
        api->bind("PushClipRect", &ImGui::PushClipRect)->setArgNames({ "min", "max", "doIntersectWithCurrent" });
        api->bind("PopClipRect", &ImGui::PopClipRect);
        // api->bind("SetItemDefaultFocus", &ImGui::SetItemDefaultFocus);
        // api->bind("SetKeyboardFocusHere", &ImGui::SetKeyboardFocusHere);
        // api->bind("SetNextItemAllowOverlap", &ImGui::SetNextItemAllowOverlap);
        // api->bind("IsItemHovered", &ImGui::IsItemHovered);
        api->bind("IsItemActive", &ImGui::IsItemActive);
        api->bind("IsItemFocused", &ImGui::IsItemFocused);
        // api->bind("IsItemClicked", &ImGui::IsItemClicked);
        api->bind("IsItemVisible", &ImGui::IsItemVisible);
        api->bind("IsItemEdited", &ImGui::IsItemEdited);
        api->bind("IsItemActivated", &ImGui::IsItemActivated);
        api->bind("IsItemDeactivated", &ImGui::IsItemDeactivated);
        api->bind("IsItemDeactivatedAfterEdit", &ImGui::IsItemDeactivatedAfterEdit);
        api->bind("IsItemToggledOpen", &ImGui::IsItemToggledOpen);
        api->bind("IsAnyItemHovered", &ImGui::IsAnyItemHovered);
        api->bind("IsAnyItemActive", &ImGui::IsAnyItemActive);
        api->bind("IsAnyItemFocused", &ImGui::IsAnyItemFocused);
        api->bind("GetItemID", &ImGui::GetItemID);
        api->bind("GetItemRectMin", &ImGui::GetItemRectMin);
        api->bind("GetItemRectMax", &ImGui::GetItemRectMax);
        api->bind("GetItemRectSize", &ImGui::GetItemRectSize);
        api->bind("GetMainViewport", &ImGui:: GetMainViewport);
        // api->bind("GetBackgroundDrawList", &ImGui::GetBackgroundDrawList);
        // api->bind("GetForegroundDrawList", &ImGui::GetForegroundDrawList);
        // api->bind("IsRectVisible", &ImGui::IsRectVisible);
        // api->bind("IsRectVisible", &ImGui::IsRectVisible);
        // api->bind("GetTime", &ImGui::GetTime);
        // api->bind("GetFrameCount", &ImGui::GetFrameCount);
        // api->bind("GetDrawListSharedData", &ImGui::GetDrawListSharedData);
        // api->bind("GetStyleColorName", &ImGui::GetStyleColorName);
        // api->bind("SetStateStorage", &ImGui::SetStateStorage);
        // api->bind("GetStateStorage", &ImGui::GetStateStorage);
        // api->bind("CalcTextSize", &ImGui::CalcTextSize);
        // api->bind("ColorConvertU32ToFloat4", &ImGui::ColorConvertU32ToFloat4);
        // api->bind("ColorConvertFloat4ToU32", &ImGui::ColorConvertFloat4ToU32);
        // api->bind("ColorConvertRGBtoHSV", &ImGui::ColorConvertRGBtoHSV);
        // api->bind("ColorConvertHSVtoRGB", &ImGui::ColorConvertHSVtoRGB);
        api->bind("IsKeyDown", &ImGui::IsKeyDown);
        api->bind("IsKeyPressed", &ImGui::IsKeyPressed);
        api->bind("IsKeyReleased", &ImGui::IsKeyReleased);
        api->bind("IsKeyChordPressed", &ImGui::IsKeyChordPressed);
        api->bind("GetKeyPressedAmount", &ImGui::GetKeyPressedAmount);
        api->bind("GetKeyName", &ImGui::GetKeyName);
        // api->bind("SetNextFrameWantCaptureKeyboard", &ImGui::SetNextFrameWantCaptureKeyboard);
        // api->bind("IsMouseDown", &ImGui::IsMouseDown);
        // api->bind("IsMouseClicked", &ImGui::IsMouseClicked);
        // api->bind("IsMouseReleased", &ImGui::IsMouseReleased);
        // api->bind("IsMouseDoubleClicked", &ImGui::IsMouseDoubleClicked);
        // api->bind("GetMouseClickedCount", &ImGui::GetMouseClickedCount);
        api->bind("IsMouseHoveringRect", &ImGui::IsMouseHoveringRect)->setArgNames({ "min", "max", "clip" });
        // api->bind("IsMousePosValid", &ImGui::IsMousePosValid);
        api->bind("IsAnyMouseDown", &ImGui::IsAnyMouseDown);
        api->bind("GetMousePos", &ImGui::GetMousePos);
        // api->bind("GetMousePosOnOpeningCurrentPopup", &ImGui::GetMousePosOnOpeningCurrentPopup);
        // api->bind("IsMouseDragging", &ImGui::IsMouseDragging);
        // api->bind("GetMouseDragDelta", &ImGui::GetMouseDragDelta);
        // api->bind("ResetMouseDragDelta", &ImGui::ResetMouseDragDelta);
        // api->bind("GetMouseCursor", &ImGui::GetMouseCursor);
        // api->bind("SetMouseCursor", &ImGui::SetMouseCursor);
        // api->bind("SetNextFrameWantCaptureMouse", &ImGui::SetNextFrameWantCaptureMouse);
        api->bind("GetClipboardText", &ImGui::GetClipboardText);
        api->bind("SetClipboardText", &ImGui::SetClipboardText);
        // api->bind("LoadIniSettingsFromDisk", &ImGui::LoadIniSettingsFromDisk);
        // api->bind("LoadIniSettingsFromMemory", &ImGui::LoadIniSettingsFromMemory);
        // api->bind("SaveIniSettingsToDisk", &ImGui::SaveIniSettingsToDisk);
        // api->bind("SaveIniSettingsToMemory", &ImGui::SaveIniSettingsToMemory);
        // api->bind("DebugTextEncoding", &ImGui::DebugTextEncoding);
        // api->bind("DebugFlashStyleColor", &ImGui::DebugFlashStyleColor);
        // api->bind("DebugStartItemPicker", &ImGui::DebugStartItemPicker);
        // api->bind("DebugCheckVersionAndDataLayout", &ImGui::DebugCheckVersionAndDataLayout);

        api->bind("BeginGlobalDockSpace", +[](bool withMenu) {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);

            ImGuiWindowFlags host_window_flags = 0;
            host_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
            host_window_flags |= ImGuiWindowFlags_NoMove;
            host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            host_window_flags |= ImGuiWindowFlags_NoBackground;
            if (withMenu) host_window_flags |= ImGuiWindowFlags_MenuBar;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("##global", nullptr, host_window_flags);
            ImGui::PopStyleVar(3);
            ImGui::DockSpace(ImGui::GetID("DockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        })->setArgNames({ "withMenu" });
        api->bind("EndGlobalDockSpace", &ImGui::End);

        // (const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness = 1.0f);
        tp_ImDrawList->bind("AddLine", &ImDrawList::AddLine)->setArgNames({ "p1", "p2", "color", "thickness" });

        // (const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding = 0.0f, ImDrawFlags flags = 0, float thickness = 1.0f);   // a: upper-left, b: lower-right (== upper-left + size)
        tp_ImDrawList->bind("AddRect", &ImDrawList::AddRect)->setArgNames({ "min", "max", "color", "rounding", "flags", "thickness" });

        // (const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding = 0.0f, ImDrawFlags flags = 0);                     // a: upper-left, b: lower-right (== upper-left + size)
        tp_ImDrawList->bind("AddRectFilled", &ImDrawList::AddRectFilled)->setArgNames({ "min", "max", "color", "rounding", "flags" });

        // (const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left);
        tp_ImDrawList->bind("AddRectFilledMultiColor", &ImDrawList::AddRectFilledMultiColor)->setArgNames({ "min", "max", "colorUpperLeft", "colorUpperRight", "colorBottomRight", "colorBottomLeft" });

        // (const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness = 1.0f);
        tp_ImDrawList->bind("AddQuad", &ImDrawList::AddQuad)->setArgNames({ "p1", "p2", "p3", "p4", "color", "thickness" });

        // (const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col);
        tp_ImDrawList->bind("AddQuadFilled", &ImDrawList::AddQuadFilled)->setArgNames({ "p1", "p2", "p3", "p4", "color" });

        // (const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness = 1.0f);
        tp_ImDrawList->bind("AddTriangle", &ImDrawList::AddTriangle)->setArgNames({ "p1", "p2", "p3", "color", "thickness" });

        // (const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col);
        tp_ImDrawList->bind("AddTriangleFilled", &ImDrawList::AddTriangleFilled)->setArgNames({ "p1", "p2", "p3", "color" });

        // (const ImVec2& center, float radius, ImU32 col, int num_segments = 0, float thickness = 1.0f);
        tp_ImDrawList->bind("AddCircle", &ImDrawList::AddCircle)->setArgNames({ "center", "radius", "color", "numSegments", "thickness" });

        // (const ImVec2& center, float radius, ImU32 col, int num_segments = 0);
        tp_ImDrawList->bind("AddCircleFilled", &ImDrawList::AddCircleFilled)->setArgNames({ "center", "radius", "color", "numSegments" });

        // (const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness = 1.0f);
        tp_ImDrawList->bind("AddNgon", &ImDrawList::AddNgon)->setArgNames({ "center", "radius", "color", "numSegments", "thickness" });

        // (const ImVec2& center, float radius, ImU32 col, int num_segments);
        tp_ImDrawList->bind("AddNgonFilled", &ImDrawList::AddNgonFilled)->setArgNames({ "center", "radius", "color", "numSegments" });

        // (const ImVec2& center, const ImVec2& radius, ImU32 col, float rot = 0.0f, int num_segments = 0, float thickness = 1.0f);
        tp_ImDrawList->bind("AddEllipse", &ImDrawList::AddEllipse)->setArgNames({ "center", "radius", "color", "rotation", "numSegments", "thickness" });

        // (const ImVec2& center, const ImVec2& radius, ImU32 col, float rot = 0.0f, int num_segments = 0);
        tp_ImDrawList->bind("AddEllipseFilled", &ImDrawList::AddEllipseFilled)->setArgNames({ "center", "radius", "color", "rotation", "numSegments" });

        // (const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL);
        struct _dummy {
            void AddText(const ImVec2& pos, ImU32 col, const char* text_begin) {
                ((ImDrawList*)this)->AddText(pos, col, text_begin);
            }
        };
        
        tp_ImDrawList->bind("AddText", &_dummy::AddText)->setArgNames({ "position", "color", "text" });

        // (const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness, int num_segments = 0); // Cubic Bezier (4 control points)
        tp_ImDrawList->bind("AddBezierCubic", &ImDrawList::AddBezierCubic)->setArgNames({ "p1", "p2", "p3", "p4", "color", "thickness", "numSegments" });

        // (const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness, int num_segments = 0);
        tp_ImDrawList->bind("AddBezierQuadratic", &ImDrawList::AddBezierQuadratic)->setArgNames({ "p1", "p2", "p3", "color", "thickness", "numSegments" });
    }
};