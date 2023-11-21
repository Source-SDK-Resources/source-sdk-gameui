#ifndef _INCLUDED_NB_HEADER_FOOTER_H
#define _INCLUDED_NB_HEADER_FOOTER_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui/VGUI.h>
#include <vgui_controls/EditablePanel.h>


// == MANAGED_CLASS_DECLARATIONS_START: Do not edit by hand ==
class vgui::Label;
// == MANAGED_CLASS_DECLARATIONS_END ==
class HUDVideoPanel;
class CNB_Gradient_Bar;

enum NB_Title_Style
{
	NB_TITLE_BRIGHT,
	NB_TITLE_MEDIUM,
};

enum NB_Background_Style
{
	NB_BACKGROUND_DARK,
	NB_BACKGROUND_TRANSPARENT_BLUE,
	NB_BACKGROUND_BLUE,
	NB_BACKGROUND_TRANSPARENT_RED,
	NB_BACKGROUND_IMAGE,
	NB_BACKGROUND_NONE,
};

class IVideoMaterial;

class CBackgroundMovie
{
public:
	CBackgroundMovie();
	~CBackgroundMovie();

	void Update();
	void SetCurrentMovie( const char *szFilename );
	int SetTextureMaterial();
	void ClearCurrentMovie();

	float MaxU() { if (m_flMaxU == 0) return 1.0f; return m_flMaxU; }
	float MaxV() { if (m_flMaxV == 0) return 1.0f; return m_flMaxV; }
	float AspectRatio() { return m_flAspectRatio; }
private:
	float m_flMaxU, m_flMaxV, m_flAspectRatio;
	IVideoMaterial* m_pMaterial;

	int m_nTextureID;
	char m_szCurrentMovie[ MAX_PATH ];
	int m_nLastGameState;
};

CBackgroundMovie* BackgroundMovie();

class CNB_Header_Footer : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CNB_Header_Footer, vgui::EditablePanel );
public:
	CNB_Header_Footer( vgui::Panel *parent, const char *name );
	virtual ~CNB_Header_Footer();
	
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void PerformLayout();
	virtual void OnThink();
	virtual void PaintBackground();
	void SetTitle( const char *pszTitle );
	void SetTitle( wchar_t *pwszTitle );
	void SetHeaderEnabled( bool bEnabled );
	void SetFooterEnabled( bool bEnabled );
	void SetTitleStyle( NB_Title_Style nTitleStyle );
	void SetBackgroundStyle( NB_Background_Style nBackgroundStyle );
	void SetMovieEnabled( bool bMovieEnabled );
	void SetGradientBarEnabled( bool bEnabled );
	void SetGradientBarPos( int nY, int nHeight );
	
	// == MANAGED_MEMBER_POINTERS_START: Do not edit by hand ==
	vgui::Panel	*m_pBackground;
	vgui::ImagePanel *m_pBackgroundImage;	
	vgui::Label	*m_pTitle;	
	vgui::Panel	*m_pTopBar;
	vgui::Panel	*m_pTopBarLine;	
	vgui::Panel	*m_pBottomBar;
	vgui::Panel	*m_pBottomBarLine;
	// == MANAGED_MEMBER_POINTERS_END ==
	HUDVideoPanel *m_pVideoPanel;
	CNB_Gradient_Bar *m_pGradientBar;

	bool m_bHeaderEnabled;
	bool m_bFooterEnabled;
	bool m_bMovieEnabled;
	bool m_bGradientBarEnabled;
	int m_nGradientBarY;
	int m_nGradientBarHeight;

	NB_Title_Style m_nTitleStyle;
	NB_Background_Style m_nBackgroundStyle;
};

class CNB_Gradient_Bar : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CNB_Gradient_Bar, vgui::EditablePanel );
public:
	CNB_Gradient_Bar( vgui::Panel *parent, const char *name );

	virtual void PaintBackground();
};

#endif // _INCLUDED_NB_HEADER_FOOTER_H
