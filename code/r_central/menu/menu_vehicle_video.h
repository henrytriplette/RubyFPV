#pragma once
#include "menu_objects.h"
#include "menu_item_select.h"
#include "menu_item_slider.h"
#include "menu_item_radio.h"
#include "menu_item_checkbox.h"
#include "menu_item_text.h"
#include "../../base/video_capture_res.h"

class MenuVehicleVideo: public Menu
{
   public:
      MenuVehicleVideo();
      virtual ~MenuVehicleVideo();
      virtual void Render();
      virtual void onReturnFromChild(int iChildMenuId, int returnValue);  
      virtual void onSelectItem();
      virtual void valuesToUI();
            
   private:
      type_video_capture_resolution_info* m_pVideoResolutions;
      int m_iVideoResolutionsCount;
      int m_IndexRes, m_IndexFPS, m_IndexKeyframe;
      int m_IndexVideoBitrate;
      int m_IndexVideoConstantBitrate;
      int m_IndexVideoProfile;
      int m_IndexVideoCodec;
      int m_IndexIgnoreTxSpikes;
      int m_IndexExpert;
      int m_IndexForceCameraMode;
      int m_IndexVideoLinkMode;
      int m_IndexBidirectionalVideoSettings;
      int m_IndexRecording;
      MenuItem* m_pMenuItemVideoWarning;
      MenuItem* m_pMenuItemVideoRecording;
      MenuItemSlider* m_pItemsSlider[15];
      MenuItemSelect* m_pItemsSelect[15];
      MenuItemRadio* m_pItemsRadio[5];

      void checkAddWarningInMenu();
      void showFPSWarning(int w, int h, int fps);
      void sendVideoLinkProfiles();
};