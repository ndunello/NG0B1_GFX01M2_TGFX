/* DO NOT EDIT THIS FILE */
/* This file is autogenerated by the text-database code generator */

#ifndef TOUCHGFX_CACHEDFONT_HPP
#define TOUCHGFX_CACHEDFONT_HPP

#include <fonts/FontCache.hpp>
#include <fonts/GeneratedFont.hpp>

namespace touchgfx
{
class CachedFont : public GeneratedFont
{
public:
    CachedFont(const struct touchgfx::BinaryFontData* data, FontId id, FontCache* _cache, const GeneratedFont* _flashFont)
        : GeneratedFont(0, //GlyphNode*
                        data->numberOfGlyphs,
                        data->height,
                        data->pixBelowBase,
                        data->bitsPerPixel,
                        data->byteAlignRow,
                        data->maxLeft,
                        data->maxRight,
                        0, //glyphDataPointer
                        0, //Kerning table not used for cached font
                        data->fallbackChar,
                        data->ellipsisChar,
                        0,  //gsubTablePointer
                        0), //contextualFormsPointer
          fontId(id),
          cache(_cache),
          flashFont(_flashFont)
    {
    }

    CachedFont()
        : GeneratedFont()
    {
    }

    using GeneratedFont::getGlyph;

    virtual const GlyphNode* getGlyph(Unicode::UnicodeChar unicode, const uint8_t*& pixelData, uint8_t& bitsPerPixel) const;

    virtual const uint8_t* getPixelData(const GlyphNode* glyph) const;

    virtual int8_t getKerning(Unicode::UnicodeChar prevChar, const GlyphNode* glyph) const;

    void setFontCache(FontCache& cache);
    FontId getFontId() const
    {
        return fontId;
    }

    virtual const uint16_t* getGSUBTable() const
    {
        if (gsubTable != 0)
        {
            return gsubTable;
        }
        return flashFont->getGSUBTable();
    }

    virtual void setGSUBTable(const uint16_t* table)
    {
        gsubTable = table;
    }

    virtual const FontContextualFormsTable* getContextualFormsTable() const
    {
        if (arabicTable != 0)
        {
            return arabicTable;
        }
        return flashFont->getContextualFormsTable();
    }

    virtual void setContextualFormsTable(const FontContextualFormsTable* table)
    {
        arabicTable = table;
    }

private:
    FontId fontId;
    FontCache* cache;
    const GeneratedFont* flashFont;
};
} // namespace touchgfx

#endif // TOUCHGFX_CACHEDFONT_HPP
