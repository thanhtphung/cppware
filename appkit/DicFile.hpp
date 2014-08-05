/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef APPKIT_DIC_FILE_HPP
#define APPKIT_DIC_FILE_HPP

#include "syskit/macros.h"

DECLARE_CLASS1(appkit, String)
DECLARE_CLASS1(appkit, StringDic)
DECLARE_CLASS1(appkit, XmlDoc)
DECLARE_CLASS1(appkit, XmlElement)

BEGIN_NAMESPACE1(appkit)


//! persistent dictionary
class DicFile
{

public:
    DicFile(const String& dicPath, bool ignoreCase = false);
    ~DicFile();

    StringDic* load() const;
    const String& dicPath() const;
    bool ignoreCase() const;
    bool isOk() const;
    bool save(const StringDic& dic);
    const char* errDesc() const;

private:
    String* dicPath_;
    XmlDoc mutable* doc_;
    bool ignoreCase_;
    bool ok_;

    DicFile(const DicFile&); //prohibit usage
    const DicFile& operator =(const DicFile&); //prohibit usage

    StringDic* loadDic() const;
    bool saveDic(const StringDic&);
    bool saveFile() const;

    static bool loadPair(StringDic&, const XmlElement&, const String&);
    static bool loadPair(void*, const XmlElement&);
    static bool savePair(StringDic&, XmlElement&, const String&, const StringDic&);
    static bool savePair(void*, XmlElement&);

};

//! Return true if case is ignored.
inline bool DicFile::ignoreCase() const
{
    return ignoreCase_;
}

//! Return true if instance was successfully constructed.
//! Use errDesc() to get an error description if necessary.
inline bool DicFile::isOk() const
{
    return ok_;
}

END_NAMESPACE1

#endif
