//========= Copyright � 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#include "VAudioVideo.h"
#include "VFooterPanel.h"
#include "VDropDownMenu.h"
#include "VSliderControl.h"
#include "EngineInterface.h"
#include "gameui_util.h"
#include "vgui/ISurface.h"
#include "VGenericConfirmation.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;
using namespace BaseModUI;

static ConVar x360_audio_english( "x360_audio_english", "0" );

//=============================================================================
AudioVideo::AudioVideo(Panel *parent, const char *panelName):
BaseClass(parent, panelName)
{
	SetDeleteSelfOnClose(true);

	SetProportional( true );

	SetUpperGarnishEnabled(true);
	SetLowerGarnishEnabled(true);

	m_sldBrightness = NULL;
	m_drpColorMode = NULL;

	m_sldGameVolume = NULL;
	m_sldMusicVolume = NULL;
	m_drpLanguage = NULL;
	m_drpCaptioning = NULL;


	// Store the old vocal language setting
	m_bOldForceEnglishAudio = x360_audio_english.GetBool();
	m_bDirtyVideoConfig = false;

	UpdateFooter();
}

//=============================================================================
AudioVideo::~AudioVideo()
{
	if ( m_bOldForceEnglishAudio != x360_audio_english.GetBool() )
	{
		PostMessage( &(CBaseModPanel::GetSingleton()), new KeyValues( "command", "command", "QuitRestartNoConfirm" ), 0.0f );
	}
}

//=============================================================================
void AudioVideo::Activate()
{
	BaseClass::Activate();

	if ( m_sldBrightness )
	{
		m_sldBrightness->Reset();
		m_sldBrightness->NavigateTo();
	}

	if ( m_drpColorMode )
	{
		ConVarRef mat_monitorgamma_tv_enabled( "mat_monitorgamma_tv_enabled" );

		if ( mat_monitorgamma_tv_enabled.GetBool() )
		{
			m_drpColorMode->SetCurrentSelection( "#L4D360UI_ColorMode_Television" );
		}
		else
		{
			m_drpColorMode->SetCurrentSelection( "#L4D360UI_ColorMode_LCD" );
		}

		FlyoutMenu *pFlyout = m_drpColorMode->GetCurrentFlyout();
		if ( pFlyout )
		{
			pFlyout->SetListener( this );
		}
	}

	if ( m_sldGameVolume )
	{
		m_sldGameVolume->Reset();
	}

	if ( m_sldMusicVolume )
	{
		m_sldMusicVolume->Reset();
	}

	if ( m_drpCaptioning )
	{
		ConVarRef closecaption("closecaption");
		ConVarRef cc_subtitles("cc_subtitles");

		if ( !closecaption.GetBool() )
		{
			m_drpCaptioning->SetCurrentSelection( "#L4D360UI_AudioOptions_CaptionOff" );
		}
		else
		{
			if ( cc_subtitles.GetBool() )
			{
				m_drpCaptioning->SetCurrentSelection( "#L4D360UI_AudioOptions_CaptionSubtitles" );
			}
			else
			{
				m_drpCaptioning->SetCurrentSelection( "#L4D360UI_AudioOptions_CaptionOn" );
			}
		}

		FlyoutMenu *pFlyout = m_drpCaptioning->GetCurrentFlyout();
		if ( pFlyout )
		{
			pFlyout->SetListener( this );
		}
	}

	if ( m_drpLanguage )
	{

		// We're in a language other than english that has full audio localization (not all of them do)
		// let them select either their native language or english for thier audio if they're not in game
		m_drpLanguage->SetFlyout( "FlmLanguage" );

		char szLanguage[ 256 ];
		Q_snprintf( szLanguage, sizeof( szLanguage ), "#GameUI_Language_%s", XBX_GetLanguageString() );

		FlyoutMenu *pFlyout = m_drpLanguage->GetCurrentFlyout();
		if ( pFlyout )
		{
			Button *pCurrentLanguageButton = pFlyout->FindChildButtonByCommand( "CurrentXBXLanguage" );

			if ( pCurrentLanguageButton )
			{
				pCurrentLanguageButton->SetText( szLanguage );
				pCurrentLanguageButton->SetCommand( szLanguage );
				pFlyout->SetListener( this );
			}
		}

		m_drpLanguage->SetCurrentSelection( ( x360_audio_english.GetBool() ) ? ( "#GameUI_Language_English" ) : ( szLanguage ) );

		// Can't change language while in game
		m_drpLanguage->SetEnabled( !engine->IsInGame() || GameUI().IsInBackgroundLevel() );
	}
	
	m_bDirtyVideoConfig = false;

	UpdateFooter();
}

void AudioVideo::OnThink()
{
	BaseClass::OnThink();

	bool needsActivate = false;

	if ( !m_sldBrightness )
	{
		m_sldBrightness = dynamic_cast< SliderControl* >( FindChildByName( "SldBrightness" ) );
		needsActivate = true;
	}

	if ( !m_drpColorMode )
	{
		m_drpColorMode = dynamic_cast< DropDownMenu* >( FindChildByName( "DrpColorMode" ) );
		needsActivate = true;
	}

	if ( !m_sldGameVolume )
	{
		m_sldGameVolume = dynamic_cast< SliderControl* >( FindChildByName( "SldGameVolume" ) );
		needsActivate = true;
	}

	if ( !m_sldMusicVolume )
	{
		m_sldMusicVolume = dynamic_cast< SliderControl* >( FindChildByName( "SldMusicVolume" ) );
		needsActivate = true;
	}

	if ( !m_drpCaptioning )
	{
		m_drpCaptioning = dynamic_cast< DropDownMenu* >( FindChildByName( "DrpCaptioning" ) );
		needsActivate = true;
	}

	if ( !m_drpLanguage )
	{
		m_drpLanguage = dynamic_cast< DropDownMenu* >( FindChildByName( "DrpLanguage" ) );
		needsActivate = true;
	}


	if ( needsActivate )
	{
		Activate();
	}
}

void AudioVideo::OnKeyCodePressed(KeyCode code)
{
	switch ( GetBaseButtonCode( code ) )
	{
	case KEY_XBUTTON_B:
		if ( m_bOldForceEnglishAudio != x360_audio_english.GetBool() )
		{
			// Pop up a dialog to confirm changing the language
			CBaseModPanel::GetSingleton().PlayUISound( UISOUND_ACCEPT );

			GenericConfirmation* confirmation = 
				static_cast< GenericConfirmation* >( CBaseModPanel::GetSingleton().OpenWindow( WT_GENERICCONFIRMATION, this, false ) );

			GenericConfirmation::Data_t data;

			data.pWindowTitle = "#GameUI_ChangeLanguageRestart_Title";
			data.pMessageText = "#GameUI_ChangeLanguageRestart_Info";

			data.bOkButtonEnabled = true;
			data.pfnOkCallback = &AcceptLanguageChangeCallback;
			data.bCancelButtonEnabled = true;
			data.pfnCancelCallback = &CancelLanguageChangeCallback;

			// WARNING! WARNING! WARNING!
			// The nature of Generic Confirmation is that it will be silently replaced
			// with another Generic Confirmation if a system event occurs
			// e.g. user unplugs controller, user changes storage device, etc.
			// If that happens neither OK nor CANCEL callbacks WILL NOT BE CALLED
			// The state machine cannot depend on either callback advancing the
			// state because in some situations neither callback can fire and the
			// confirmation dismissed/closed/replaced.
			// State machine must implement OnThink and check if the required
			// confirmation box is still present!
			// This code implements no fallback - it will result in minor UI
			// bug that the language box will be changed, but the title not restarted.
			// Vitaliy -- 9/26/2009
			//
			confirmation->SetUsageData(data);
		}
		else
		{
			if ( m_bDirtyVideoConfig || 
				( m_sldBrightness && m_sldBrightness->IsDirty() ) ||
				( m_sldGameVolume && m_sldGameVolume->IsDirty() ) || 
				( m_sldMusicVolume && m_sldMusicVolume->IsDirty() ) )
			{
				// Want to write data
				if ( CBaseModPanel::GetSingleton().IsReadyToWriteConfig() )
				{
					// Ready to write that data... go ahead and nav back
					BaseClass::OnKeyCodePressed(code);
				}
			}
			else
			{
				// Don't need to write data... go ahead and nav back
				BaseClass::OnKeyCodePressed(code);
			}
		}
		break;

	default:
		BaseClass::OnKeyCodePressed(code);
		break;
	}
}

//=============================================================================
void AudioVideo::OnCommand(const char *command)
{
	if ( !Q_stricmp( command, "#L4D360UI_ColorMode_Television" ) )
	{
		ConVarRef mat_monitorgamma_tv_enabled( "mat_monitorgamma_tv_enabled" );
		mat_monitorgamma_tv_enabled.SetValue( 1 );
		m_bDirtyVideoConfig = true;
	}
	else if ( !Q_stricmp( command, "#L4D360UI_ColorMode_LCD" ) )
	{
		ConVarRef mat_monitorgamma_tv_enabled( "mat_monitorgamma_tv_enabled" );
		mat_monitorgamma_tv_enabled.SetValue( 0 );
		m_bDirtyVideoConfig = true;
	}
	else if ( Q_stricmp( "#L4D360UI_AudioOptions_CaptionOff", command ) == 0 )
	{
		ConVarRef closecaption("closecaption");
		ConVarRef cc_subtitles("cc_subtitles");
		closecaption.SetValue( 0 );
		cc_subtitles.SetValue( 0 );
		m_bDirtyVideoConfig = true;
	}
	else if ( Q_stricmp( "#L4D360UI_AudioOptions_CaptionSubtitles", command ) == 0 )
	{
		ConVarRef closecaption("closecaption");
		ConVarRef cc_subtitles("cc_subtitles");
		closecaption.SetValue( 1 );
		cc_subtitles.SetValue( 1 );
		m_bDirtyVideoConfig = true;
	}
	else if ( Q_stricmp( "#L4D360UI_AudioOptions_CaptionOn", command ) == 0 )
	{
		ConVarRef closecaption("closecaption");
		ConVarRef cc_subtitles("cc_subtitles");
		closecaption.SetValue( 1 );
		cc_subtitles.SetValue( 0 );
		m_bDirtyVideoConfig = true;
	}
	else if ( StringHasPrefix( command, "#GameUI_Language_" ) )
	{
		const char *pchLanguage = StringAfterPrefix( command, "#GameUI_Language_" );

		// Decide whether we selected English or the other
		x360_audio_english.SetValue( Q_strcmp( pchLanguage, "English" ) == 0 );
	}

	else
	{
		BaseClass::OnCommand( command );
	}
}

void AudioVideo::OnNotifyChildFocus( vgui::Panel* child )
{
}

void AudioVideo::UpdateFooter()
{
	CBaseModFooterPanel *footer = BaseModUI::CBaseModPanel::GetSingleton().GetFooterPanel();
	if ( footer )
	{
		footer->SetButtons( FB_ABUTTON | FB_BBUTTON, FF_AB_ONLY, true );
		footer->SetButtonText( FB_ABUTTON, "#L4D360UI_Select" );
		footer->SetButtonText( FB_BBUTTON, "#L4D360UI_Controller_Done" );
	}
}

void AudioVideo::OnFlyoutMenuClose( vgui::Panel* flyTo )
{
	UpdateFooter();
}

void AudioVideo::OnFlyoutMenuCancelled()
{
}

//=============================================================================
Panel* AudioVideo::NavigateBack()
{
	// For cert we can only write one config in this spot!! (and have to wait 3 seconds before writing another)
	if ( m_bDirtyVideoConfig || 
		 ( m_sldBrightness && m_sldBrightness->IsDirty() ) ||
		 ( m_sldGameVolume && m_sldGameVolume->IsDirty() ) || 
		 ( m_sldMusicVolume && m_sldMusicVolume->IsDirty() ) )
	{
		// Write only video config
		if ( CBaseModPanel::GetSingleton().IsReadyToWriteConfig() )
		{
			engine->ClientCmd_Unrestricted( "host_writeconfig_video" );
		}
	}

	return BaseClass::NavigateBack();
}

void AudioVideo::ResetLanguage()
{
	if ( m_drpLanguage )
	{
		// Reset to the setting we started the game with
		char szLanguage[ 256 ];
		Q_snprintf( szLanguage, sizeof( szLanguage ), "#GameUI_Language_%s", XBX_GetLanguageString() );

		m_drpLanguage->SetCurrentSelection( ( m_bOldForceEnglishAudio ) ? ( "#GameUI_Language_English" ) : ( szLanguage ) );
	}
}

void AudioVideo::AcceptLanguageChangeCallback() 
{
	AudioVideo *self = static_cast< AudioVideo * >( CBaseModPanel::GetSingleton().GetWindow( WT_AUDIOVIDEO ) );
	if( self )
	{
		self->BaseClass::OnKeyCodePressed( KEY_XBUTTON_B );
	}
}

//=============================================================================
void AudioVideo::CancelLanguageChangeCallback()
{
	AudioVideo *self = static_cast< AudioVideo * >( CBaseModPanel::GetSingleton().GetWindow( WT_AUDIOVIDEO ) );
	if( self )
	{
		self->ResetLanguage();
	}
}

void AudioVideo::PaintBackground()
{
	BaseClass::DrawDialogBackground( "#L4D360UI_AudioVideo_Title", NULL, "#L4D360UI_AudioVideo_Desc", NULL );
}

void AudioVideo::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	// required for new style
	SetPaintBackgroundEnabled( true );
	SetupAsDialogStyle();
}
