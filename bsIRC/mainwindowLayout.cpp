m_pcRoot = new os::VLayoutNode( "Root" );
m_pcVRoot = new os::VLayoutNode( "VRoot", 1.000000 );
m_pcVRoot->SetBorders( os::Rect( 0.000000, 0.000000, 0.000000, 0.000000 ) );
m_pcRoot->AddChild( m_pcVRoot );
m_pcMenu = new os::Menu( os::Rect(), "Menu", os::ITEMS_IN_ROW );
os::Menu* pcMenuApplicationMenu = new os::Menu( os::Rect(), "Application", os::ITEMS_IN_COLUMN );
pcMenuApplicationMenu->AddItem( "Quit", new Message( M_MENU_QUIT ) );
pcMenuApplicationMenu->AddItem( new MenuSeparator() );
pcMenuApplicationMenu->AddItem( "Help...", new Message( M_MENU_HELP ) );
pcMenuApplicationMenu->AddItem( "Website...", new Message( M_MENU_WEB ) );
pcMenuApplicationMenu->AddItem( new MenuSeparator() );
pcMenuApplicationMenu->AddItem( "About...", new Message( M_MENU_ABOUT ) );
m_pcMenu->AddItem( pcMenuApplicationMenu );
m_pcMenu->SetTargetForItems(this);
m_pcVRoot->AddChild( m_pcMenu, 1.000000 );
m_pcHLayoutNode = new os::HLayoutNode( "HLayoutNode", 1.000000 );
m_pcHLayoutNode->SetBorders( os::Rect( 5.000000, 5.000000, 5.000000, 5.000000 ) );
m_pcVRoot->AddChild( m_pcHLayoutNode );
m_pcSideNode = new os::VLayoutNode( "SideNode", 1.000000 );
m_pcSideNode->SetBorders( os::Rect( 0.000000, 0.000000, 5.000000, 0.000000 ) );
m_pcHLayoutNode->AddChild( m_pcSideNode );
m_pcSideTab = new os::TabView( os::Rect(), "SideTab" );
m_pcBookmarksTab = new os::LayoutView( os::Rect(), "BookmarksTab" );
m_pcBookmarksLayout = new os::VLayoutNode( "BookmarksLayout" );
m_pcSideTab->AppendTab(  "Bookmarks", m_pcBookmarksTab );
m_pcUserListTab = new os::LayoutView( os::Rect(), "UserListTab" );
m_pcUserListLayout = new os::VLayoutNode( "UserListLayout" );
m_pcSideTab->AppendTab(  "UserList", m_pcUserListTab );
m_pcSideNode->AddChild( m_pcSideTab, 1.000000 );
m_pcBookmarks = new os::VLayoutNode( "Bookmarks", 1.000000 );
m_pcBookmarks->SetBorders( os::Rect( 3.000000, 0.000000, 3.000000, 3.000000 ) );
m_pcBookmarksLayout->AddChild( m_pcBookmarks );
m_pcBookmarkFrame = new os::FrameView( os::Rect(), "BookmarkFrame", "Add Bookmarks" );
m_pcBookmarkFrameLayout = new os::VLayoutNode( "BookmarkFrameLayout" );
m_pcBookmarkFrameLayout->SetBorders( os::Rect( 3.000000, 1.000000, 3.000000, 3.000000 ) );
m_pcBookmarks->AddChild( m_pcBookmarkFrame, 1.000000 );
m_pcTextBookmarkName = new os::TextView( os::Rect(), "TextBookmarkName", "" );
m_pcBookmarkFrameLayout->AddChild( m_pcTextBookmarkName, 1.000000 );
m_pcBookmarkSpacer1 = new os::VLayoutSpacer( "BookmarkSpacer1", 3.000000, 3.000000, NULL, 1.000000 );
m_pcBookmarkFrameLayout->AddChild( m_pcBookmarkSpacer1 );
m_pcButtonBookmarkName = new os::Button( os::Rect(), "ButtonBookmarkName", "Add Bookmark", new os::Message( M_BUTTON_ADDBOOKMARK ) );
m_pcBookmarkFrameLayout->AddChild( m_pcButtonBookmarkName, 1.000000 );
m_pcBookmarkFrame->SetRoot( m_pcBookmarkFrameLayout );
m_pcBookmarkSpacer2 = new os::VLayoutSpacer( "BookmarkSpacer2", 10.000000, 10.000000, NULL, 1.000000 );
m_pcBookmarks->AddChild( m_pcBookmarkSpacer2 );
m_pcBookmarkList = new os::ListView( os::Rect(), "BookmarkList", os::ListView::F_NO_HEADER );
m_pcBookmarkList->InsertColumn( "Name", 1 );
m_pcBookmarks->AddChild( m_pcBookmarkList, 50.000000 );
m_pcBookmarkSpacer3 = new os::VLayoutSpacer( "BookmarkSpacer3", 10.000000, 10.000000, NULL, 1.000000 );
m_pcBookmarks->AddChild( m_pcBookmarkSpacer3 );
m_pcBookmarkButtonsNode = new os::HLayoutNode( "BookmarkButtonsNode", 1.000000 );
m_pcBookmarkButtonsNode->SetBorders( os::Rect( 1.000000, 0.000000, 1.000000, 0.000000 ) );
m_pcBookmarks->AddChild( m_pcBookmarkButtonsNode );
os::File cButtonRemoveBookmarkFile( open_image_file( get_image_id() ) );
os::Resources cButtonRemoveBookmarkResources( &cButtonRemoveBookmarkFile );
os::ResStream* pcButtonRemoveBookmarkStream = cButtonRemoveBookmarkResources.GetResourceStream( "remove.png" );
m_pcButtonRemoveBookmark = new os::ImageButton( os::Rect(), "ButtonRemoveBookmark", "", new os::Message( M_BUTTON_REMOVEBOOKMARK ), new os::BitmapImage( pcButtonRemoveBookmarkStream ), os::ImageButton::IB_TEXT_BOTTOM, true, true, true );
delete( pcButtonRemoveBookmarkStream );
m_pcBookmarkButtonsNode->AddChild( m_pcButtonRemoveBookmark, 1.000000 );
m_pcBookmarkSpacer4 = new os::HLayoutSpacer( "BookmarkSpacer4", 10.000000, 10.000000, NULL, 1.000000 );
m_pcBookmarkButtonsNode->AddChild( m_pcBookmarkSpacer4 );
m_pcButtonBConnect = new os::Button( os::Rect(), "ButtonBConnect", "Connect", new os::Message( M_BUTTON_BCONNECT ) );
m_pcBookmarkButtonsNode->AddChild( m_pcButtonBConnect, 3.000000 );
m_pcUserList = new os::VLayoutNode( "UserList", 1.000000 );
m_pcUserList->SetBorders( os::Rect( 0.000000, 0.000000, 0.000000, 0.000000 ) );
m_pcUserListLayout->AddChild( m_pcUserList );
m_pcBookmarksTab->SetRoot( m_pcBookmarksLayout );
m_pcUserListTab->SetRoot( m_pcUserListLayout );
m_pcMainNode = new os::VLayoutNode( "MainNode", 50.000000 );
m_pcMainNode->SetBorders( os::Rect( 5.000000, 0.000000, 0.000000, 0.000000 ) );
m_pcHLayoutNode->AddChild( m_pcMainNode );
m_pcConnectNode = new os::HLayoutNode( "ConnectNode", 1.000000 );
m_pcConnectNode->SetBorders( os::Rect( 0.000000, 0.000000, 0.000000, 10.000000 ) );
m_pcMainNode->AddChild( m_pcConnectNode );
m_pcConnectTab = new os::TabView( os::Rect(), "ConnectTab" );
m_pcUserSettingsTab = new os::LayoutView( os::Rect(), "UserSettingsTab" );
m_pcUserSettingsLayout = new os::VLayoutNode( "UserSettingsLayout" );
m_pcConnectTab->AppendTab(  "UserSettings", m_pcUserSettingsTab );
m_pcServerSettingsTab = new os::LayoutView( os::Rect(), "ServerSettingsTab" );
m_pcServerSettingsLayout = new os::VLayoutNode( "ServerSettingsLayout" );
m_pcConnectTab->AppendTab(  "ServerSettings", m_pcServerSettingsTab );
m_pcConnectNode->AddChild( m_pcConnectTab, 25.000000 );
m_pcUserSettings = new os::HLayoutNode( "UserSettings", 1.000000 );
m_pcUserSettings->SetBorders( os::Rect( 0.000000, 0.000000, 0.000000, 3.000000 ) );
m_pcUserSettingsLayout->AddChild( m_pcUserSettings );
m_pcUserNameNode = new os::VLayoutNode( "UserNameNode", 2.000000 );
m_pcUserNameNode->SetBorders( os::Rect( 5.000000, 0.000000, 15.000000, 0.000000 ) );
m_pcUserSettings->AddChild( m_pcUserNameNode );
m_pcStringUserName = new os::StringView( os::Rect(), "StringUserName", "Nickname:" );
m_pcUserNameNode->AddChild( m_pcStringUserName, 1.000000 );
m_pcUserNameSpacer = new os::VLayoutSpacer( "UserNameSpacer", 5.000000, 5.000000, NULL, 1.000000 );
m_pcUserNameNode->AddChild( m_pcUserNameSpacer );
m_pcTextUserName = new os::TextView( os::Rect(), "TextUserName", "" );
m_pcUserNameNode->AddChild( m_pcTextUserName, 1.000000 );
m_pcUserPasswordNode = new os::VLayoutNode( "UserPasswordNode", 1.000000 );
m_pcUserPasswordNode->SetBorders( os::Rect( 15.000000, 0.000000, 15.000000, 0.000000 ) );
m_pcUserSettings->AddChild( m_pcUserPasswordNode );
m_pcStringUserPassword = new os::StringView( os::Rect(), "StringUserPassword", "Password:" );
m_pcUserPasswordNode->AddChild( m_pcStringUserPassword, 1.000000 );
m_pcUserPasswordSpacer = new os::VLayoutSpacer( "UserPasswordSpacer", 5.000000, 5.000000, NULL, 1.000000 );
m_pcUserPasswordNode->AddChild( m_pcUserPasswordSpacer );
m_pcTextUserPassword = new os::TextView( os::Rect(), "TextUserPassword", "" );
m_pcTextUserPassword->SetPasswordMode( true );
m_pcUserPasswordNode->AddChild( m_pcTextUserPassword, 1.000000 );
m_pcUserRealNameNode = new os::VLayoutNode( "UserRealNameNode", 2.000000 );
m_pcUserRealNameNode->SetBorders( os::Rect( 15.000000, 0.000000, 5.000000, 0.000000 ) );
m_pcUserSettings->AddChild( m_pcUserRealNameNode );
m_pcStringUserRealName = new os::StringView( os::Rect(), "StringUserRealName", "Real Name:" );
m_pcUserRealNameNode->AddChild( m_pcStringUserRealName, 1.000000 );
m_pcUserRealNameSpacer = new os::VLayoutSpacer( "UserRealNameSpacer", 5.000000, 5.000000, NULL, 1.000000 );
m_pcUserRealNameNode->AddChild( m_pcUserRealNameSpacer );
m_pcTextUserRealName = new os::TextView( os::Rect(), "TextUserRealName", "" );
m_pcUserRealNameNode->AddChild( m_pcTextUserRealName, 1.000000 );
m_pcServerSettings = new os::HLayoutNode( "ServerSettings", 1.000000 );
m_pcServerSettings->SetBorders( os::Rect( 0.000000, 0.000000, 0.000000, 3.000000 ) );
m_pcServerSettingsLayout->AddChild( m_pcServerSettings );
m_pcServerNameNode = new os::VLayoutNode( "ServerNameNode", 2.000000 );
m_pcServerNameNode->SetBorders( os::Rect( 5.000000, 0.000000, 15.000000, 0.000000 ) );
m_pcServerSettings->AddChild( m_pcServerNameNode );
m_pcStringServerName = new os::StringView( os::Rect(), "StringServerName", "Server:" );
m_pcServerNameNode->AddChild( m_pcStringServerName, 1.000000 );
m_pcServerNameSpacer = new os::VLayoutSpacer( "ServerNameSpacer", 5.000000, 5.000000, NULL, 1.000000 );
m_pcServerNameNode->AddChild( m_pcServerNameSpacer );
m_pcTextServerName = new os::TextView( os::Rect(), "TextServerName", "" );
m_pcServerNameNode->AddChild( m_pcTextServerName, 1.000000 );
m_pcServerPortNode = new os::VLayoutNode( "ServerPortNode", 1.000000 );
m_pcServerPortNode->SetBorders( os::Rect( 15.000000, 0.000000, 15.000000, 0.000000 ) );
m_pcServerSettings->AddChild( m_pcServerPortNode );
m_pcStringServerPort = new os::StringView( os::Rect(), "StringServerPort", "Port:" );
m_pcServerPortNode->AddChild( m_pcStringServerPort, 1.000000 );
m_pcServerPortSpacer = new os::VLayoutSpacer( "ServerPortSpacer", 5.000000, 5.000000, NULL, 1.000000 );
m_pcServerPortNode->AddChild( m_pcServerPortSpacer );
m_pcTextServerPort = new os::TextView( os::Rect(), "TextServerPort", "6667" );
m_pcServerPortNode->AddChild( m_pcTextServerPort, 1.000000 );
m_pcServerChannelNode = new os::VLayoutNode( "ServerChannelNode", 2.000000 );
m_pcServerChannelNode->SetBorders( os::Rect( 15.000000, 0.000000, 5.000000, 0.000000 ) );
m_pcServerSettings->AddChild( m_pcServerChannelNode );
m_pcStringServerChannel = new os::StringView( os::Rect(), "StringServerChannel", "Channel:" );
m_pcServerChannelNode->AddChild( m_pcStringServerChannel, 1.000000 );
m_pcServerChannelSpacer = new os::VLayoutSpacer( "ServerChannelSpacer", 5.000000, 5.000000, NULL, 1.000000 );
m_pcServerChannelNode->AddChild( m_pcServerChannelSpacer );
m_pcTextServerChannel = new os::TextView( os::Rect(), "TextServerChannel", "" );
m_pcServerChannelNode->AddChild( m_pcTextServerChannel, 1.000000 );
m_pcUserSettingsTab->SetRoot( m_pcUserSettingsLayout );
m_pcServerSettingsTab->SetRoot( m_pcServerSettingsLayout );
m_pcConnectButtonsNode = new os::VLayoutNode( "ConnectButtonsNode", 1.000000 );
m_pcConnectButtonsNode->SetBorders( os::Rect( 10.000000, 0.000000, 0.000000, 0.000000 ) );
m_pcConnectNode->AddChild( m_pcConnectButtonsNode );
m_pcConnectSpacer1 = new os::VLayoutSpacer( "ConnectSpacer1", 12.000000, 12.000000, NULL, 1.000000 );
m_pcConnectButtonsNode->AddChild( m_pcConnectSpacer1 );
m_pcButtonConnect = new os::Button( os::Rect(), "ButtonConnect", "Connect", new os::Message( M_BUTTON_CCONNECT ) );
m_pcConnectButtonsNode->AddChild( m_pcButtonConnect, 1.000000 );
m_pcConnectSpacer2 = new os::VLayoutSpacer( "ConnectSpacer2", 5.000000, 5.000000, NULL, 1.000000 );
m_pcConnectButtonsNode->AddChild( m_pcConnectSpacer2 );
m_pcButtonDisconnect = new os::Button( os::Rect(), "ButtonDisconnect", "Disconnect", new os::Message( M_BUTTON_DISCONNECT ) );
m_pcConnectButtonsNode->AddChild( m_pcButtonDisconnect, 1.000000 );
m_pcTextView = new os::TextView( os::Rect(), "TextView", "" );
m_pcTextView->SetMultiLine( true );
m_pcTextView->SetReadOnly( true );
m_pcMainNode->AddChild( m_pcTextView, 35.000000 );
m_pcMessageNode = new os::HLayoutNode( "MessageNode", 2.100000 );
m_pcMessageNode->SetBorders( os::Rect( 0.000000, 10.000000, 0.000000, 0.000000 ) );
m_pcMainNode->AddChild( m_pcMessageNode );
m_pcMessageText = new os::TextView( os::Rect(), "MessageText", "" );
m_pcMessageText->SetMultiLine( true );
m_pcMessageNode->AddChild( m_pcMessageText, 25.000000 );
m_pcMessageButtonsNode = new os::VLayoutNode( "MessageButtonsNode", 1.000000 );
m_pcMessageButtonsNode->SetBorders( os::Rect( 10.000000, 0.000000, 0.000000, 0.000000 ) );
m_pcMessageNode->AddChild( m_pcMessageButtonsNode );
m_pcButtonMessageSend = new os::Button( os::Rect(), "ButtonMessageSend", "Send", new os::Message( M_BUTTON_SEND ) );
m_pcMessageButtonsNode->AddChild( m_pcButtonMessageSend, 5.000000 );
m_pcMessageSpacer = new os::VLayoutSpacer( "MessageSpacer", 10.000000, 10.000000, NULL, 1.000000 );
m_pcMessageButtonsNode->AddChild( m_pcMessageSpacer );
m_pcButtonTextClear = new os::Button( os::Rect(), "ButtonTextClear", "Clear", new os::Message( M_BUTTON_CLEAR ) );
m_pcMessageButtonsNode->AddChild( m_pcButtonTextClear, 1.000000 );
