/***************************************************************
                         frametags.cpp

TODO
***************************************************************/

#include "frametags.h"

FrameTags::FrameTags( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxFlexGridSizer* m_Sizer_Tags;
    m_Sizer_Tags = new wxFlexGridSizer( 0, 1, 0, 0 );
    m_Sizer_Tags->AddGrowableCol( 0 );
    m_Sizer_Tags->AddGrowableRow( 14 );
    m_Sizer_Tags->SetFlexibleDirection( wxVERTICAL );
    m_Sizer_Tags->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    wxStaticText* m_StaticText_SDK;
    m_StaticText_SDK = new wxStaticText( this, wxID_ANY, wxT("SDK:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_StaticText_SDK->Wrap( -1 );
    m_Sizer_Tags->Add( m_StaticText_SDK, 0, wxALL, 5 );

    wxWrapSizer* m_Sizer_SDK;
    m_Sizer_SDK = new wxWrapSizer( wxHORIZONTAL, wxWRAPSIZER_DEFAULT_FLAGS );

    m_CheckBox_SDKLibultra = new wxCheckBox( this, wxID_ANY, wxT("Libultra"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_SDK->Add( m_CheckBox_SDKLibultra, 0, wxALL, 5 );

    m_CheckBox_SDKLibdragon = new wxCheckBox( this, wxID_ANY, wxT("Libdragon"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_SDK->Add( m_CheckBox_SDKLibdragon, 0, wxALL, 5 );

    m_CheckBox_SDKOther = new wxCheckBox( this, wxID_ANY, wxT("Other"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_SDK->Add( m_CheckBox_SDKOther, 0, wxALL, 5 );


    m_Sizer_Tags->Add( m_Sizer_SDK, 1, wxEXPAND, 5 );

    wxStaticText* m_StaticText_Requirements;
    m_StaticText_Requirements = new wxStaticText( this, wxID_ANY, wxT("Requirements:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_StaticText_Requirements->Wrap( -1 );
    m_Sizer_Tags->Add( m_StaticText_Requirements, 0, wxALL, 5 );

    wxWrapSizer* m_Sizer_Requirements;
    m_Sizer_Requirements = new wxWrapSizer( wxHORIZONTAL, wxWRAPSIZER_DEFAULT_FLAGS );

    m_CheckBox_RequirementsEPak = new wxCheckBox( this, wxID_ANY, wxT("Expansion Pak"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Requirements->Add( m_CheckBox_RequirementsEPak, 0, wxALL, 5 );


    m_Sizer_Tags->Add( m_Sizer_Requirements, 1, wxEXPAND, 5 );

    wxStaticText* m_StaticText_Support;
    m_StaticText_Support = new wxStaticText( this, wxID_ANY, wxT("Support:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_StaticText_Support->Wrap( -1 );
    m_Sizer_Tags->Add( m_StaticText_Support, 0, wxALL, 5 );

    wxWrapSizer* m_Sizer_Support;
    m_Sizer_Support = new wxWrapSizer( wxHORIZONTAL, 0 );

    m_CheckBox_SupportNTSC = new wxCheckBox( this, wxID_ANY, wxT("NTSC"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Support->Add( m_CheckBox_SupportNTSC, 0, wxALL, 5 );

    m_CheckBox_SupportPAL = new wxCheckBox( this, wxID_ANY, wxT("PAL"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Support->Add( m_CheckBox_SupportPAL, 0, wxALL, 5 );

    m_CheckBox_SupportMPAL = new wxCheckBox( this, wxID_ANY, wxT("MPAL"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Support->Add( m_CheckBox_SupportMPAL, 0, wxALL, 5 );

    m_CheckBox_Support64Drive = new wxCheckBox( this, wxID_ANY, wxT("64Drive"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Support->Add( m_CheckBox_Support64Drive, 0, wxALL, 5 );

    m_CheckBox_SupportED = new wxCheckBox( this, wxID_ANY, wxT("EverDrive"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Support->Add( m_CheckBox_SupportED, 0, wxALL, 5 );

    m_CheckBox_SupportSC64 = new wxCheckBox( this, wxID_ANY, wxT("SummerCart64"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Support->Add( m_CheckBox_SupportSC64, 0, wxALL, 5 );

    m_CheckBox_InaccurateEmulator = new wxCheckBox( this, wxID_ANY, wxT("Inaccurate Emulators"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Support->Add( m_CheckBox_InaccurateEmulator, 0, wxALL, 5 );

    m_CheckBox_AccurateEmulator = new wxCheckBox( this, wxID_ANY, wxT("Accurate Emulators"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Support->Add( m_CheckBox_AccurateEmulator, 0, wxALL, 5 );


    m_Sizer_Tags->Add( m_Sizer_Support, 1, wxEXPAND, 5 );

    wxStaticText* m_StaticText_Players;
    m_StaticText_Players = new wxStaticText( this, wxID_ANY, wxT("Player Count:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_StaticText_Players->Wrap( -1 );
    m_Sizer_Tags->Add( m_StaticText_Players, 0, wxALL, 5 );

    wxWrapSizer* m_Sizer_Players;
    m_Sizer_Players = new wxWrapSizer( wxHORIZONTAL, wxWRAPSIZER_DEFAULT_FLAGS );

    m_CheckBox_Players1 = new wxCheckBox( this, wxID_ANY, wxT("1 Player"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Players->Add( m_CheckBox_Players1, 0, wxALL, 5 );

    m_CheckBox_Players2 = new wxCheckBox( this, wxID_ANY, wxT("2 Player"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Players->Add( m_CheckBox_Players2, 0, wxALL, 5 );

    m_CheckBox_Players3 = new wxCheckBox( this, wxID_ANY, wxT("3 Player"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Players->Add( m_CheckBox_Players3, 0, wxALL, 5 );

    m_CheckBox_Players4 = new wxCheckBox( this, wxID_ANY, wxT("4 Player"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Players->Add( m_CheckBox_Players4, 0, wxALL, 5 );

    m_CheckBox_Players0 = new wxCheckBox( this, wxID_ANY, wxT("Video"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Players->Add( m_CheckBox_Players0, 0, wxALL, 5 );

    m_CheckBox_PlayersMulti = new wxCheckBox( this, wxID_ANY, wxT("Online Multiplayer"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Players->Add( m_CheckBox_PlayersMulti, 0, wxALL, 5 );


    m_Sizer_Tags->Add( m_Sizer_Players, 1, wxEXPAND, 5 );

    wxStaticText* m_StaticText_Accessories;
    m_StaticText_Accessories = new wxStaticText( this, wxID_ANY, wxT("Accessories:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_StaticText_Accessories->Wrap( -1 );
    m_Sizer_Tags->Add( m_StaticText_Accessories, 0, wxALL, 5 );

    wxWrapSizer* m_Sizer_Accessories;
    m_Sizer_Accessories = new wxWrapSizer( wxHORIZONTAL, wxWRAPSIZER_DEFAULT_FLAGS );

    m_CheckBox_AccessoriesRumble = new wxCheckBox( this, wxID_ANY, wxT("Rumble Pak"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Accessories->Add( m_CheckBox_AccessoriesRumble, 0, wxALL, 5 );

    m_CheckBox_AccessoriesTransfer = new wxCheckBox( this, wxID_ANY, wxT("Transfer Pak"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Accessories->Add( m_CheckBox_AccessoriesTransfer, 0, wxALL, 5 );

    m_CheckBox_AccessoriesMouse = new wxCheckBox( this, wxID_ANY, wxT("Mouse"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Accessories->Add( m_CheckBox_AccessoriesMouse, 0, wxALL, 5 );

    m_CheckBox_AccessoriesVRU = new wxCheckBox( this, wxID_ANY, wxT("VRU"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Accessories->Add( m_CheckBox_AccessoriesVRU, 0, wxALL, 5 );

    m_CheckBox_AccessoriesGCCont = new wxCheckBox( this, wxID_ANY, wxT("GameCube"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Accessories->Add( m_CheckBox_AccessoriesGCCont, 0, wxALL, 5 );

    m_CheckBox_AccessoriesGCKeyb = new wxCheckBox( this, wxID_ANY, wxT("GameCube Keyboard"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Accessories->Add( m_CheckBox_AccessoriesGCKeyb, 0, wxALL, 5 );

    m_CheckBox_AccessoriesRandnet = new wxCheckBox( this, wxID_ANY, wxT("Randnet Keyboard"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Accessories->Add( m_CheckBox_AccessoriesRandnet, 0, wxALL, 5 );


    m_Sizer_Tags->Add( m_Sizer_Accessories, 1, wxEXPAND, 5 );

    wxStaticText* m_StaticText_Saving;
    m_StaticText_Saving = new wxStaticText( this, wxID_ANY, wxT("Saving:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_StaticText_Saving->Wrap( -1 );
    m_Sizer_Tags->Add( m_StaticText_Saving, 0, wxALL, 5 );

    wxWrapSizer* m_Sizer_Saving;
    m_Sizer_Saving = new wxWrapSizer( wxHORIZONTAL, wxWRAPSIZER_DEFAULT_FLAGS );

    m_CheckBox_SavingEEPROM = new wxCheckBox( this, wxID_ANY, wxT("EEPROM"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Saving->Add( m_CheckBox_SavingEEPROM, 0, wxALL, 5 );

    m_CheckBox_SavingSRAM = new wxCheckBox( this, wxID_ANY, wxT("SRAM"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Saving->Add( m_CheckBox_SavingSRAM, 0, wxALL, 5 );

    m_CheckBox_SavingFlashRAM = new wxCheckBox( this, wxID_ANY, wxT("FlashRAM"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Saving->Add( m_CheckBox_SavingFlashRAM, 0, wxALL, 5 );

    m_CheckBox_SavingCPak = new wxCheckBox( this, wxID_ANY, wxT("Controller Pak"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Saving->Add( m_CheckBox_SavingCPak, 0, wxALL, 5 );


    m_Sizer_Tags->Add( m_Sizer_Saving, 1, wxEXPAND, 5 );

    wxStaticText* m_StaticText_Other;
    m_StaticText_Other = new wxStaticText( this, wxID_ANY, wxT("Other:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_StaticText_Other->Wrap( -1 );
    m_Sizer_Tags->Add( m_StaticText_Other, 0, wxALL, 5 );

    wxWrapSizer* m_Sizer_Other;
    m_Sizer_Other = new wxWrapSizer( wxHORIZONTAL, wxWRAPSIZER_DEFAULT_FLAGS );

    m_CheckBox_OtherSource = new wxCheckBox( this, wxID_ANY, wxT("Source Available"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Other->Add( m_CheckBox_OtherSource, 0, wxALL, 5 );

    m_CheckBox_OtherRTC = new wxCheckBox( this, wxID_ANY, wxT("Realtime Clock"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Other->Add( m_CheckBox_OtherRTC, 0, wxALL, 5 );

    m_CheckBox_OtherEmbeddedData = new wxCheckBox( this, wxID_ANY, wxT("Embedded Metadata"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Other->Add( m_CheckBox_OtherEmbeddedData, 0, wxALL, 5 );


    m_Sizer_Tags->Add( m_Sizer_Other, 1, wxEXPAND, 5 );

    m_Button_Ok = new wxButton( this, wxID_ANY, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Tags->Add( m_Button_Ok, 0, wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );


    this->SetSizer( m_Sizer_Tags );
    this->Layout();

    this->Centre( wxBOTH );
}

FrameTags::~FrameTags()
{

}