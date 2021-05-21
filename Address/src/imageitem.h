#ifndef IMAGEITEMH
#define IMAGEITEMH

#include <gui/menu.h>
#include <gui/bitmap.h>
#include <gui/font.h>

#define SUB_MENU_ARROW_W 10
#define SUB_MENU_ARROW_H 10




using namespace os;

class ImageItem : public MenuItem
{
public:
    ImageItem( const char* pzLabel, Message* pcMsg, const char *shortcut = NULL, Bitmap *bmap = NULL);
    ImageItem( Menu* pcMenu, Message* pcMsg = NULL, Bitmap *bmap = NULL);
    ~ImageItem();

    virtual void  Draw();
    virtual void  DrawContent();
    virtual void  Highlight( bool bHighlight );
    virtual Point GetContentSize();

    void SetBitmap(Bitmap *bm);
    Bitmap *GetBitmap()
    {
        return m_Bitmap;
    }

    void SetShortcut(const char *shortcut);
    const char *GetShortcut()
    {
        return m_Shortcut;
    }

private:

    void Init();

    bool	m_Enabled;
    bool	m_Highlighted;
    bool	m_HasIcon;
    int	m_IconWidth;
    int	m_IconHeight;
    char	*m_Shortcut;
    Bitmap	*m_Bitmap;
};


#endif











