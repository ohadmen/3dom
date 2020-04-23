#include "zview/io/file_types/read_stl.h"
#include "zview/io/file_types/read_obj.h"
#include "zview/io/file_types/read_ply.h"

#include "read_file_list.h"
#include <QtWidgets/QFileDialog>
#include <QtCore/QDebug>
std::vector <std::pair<std::string,Types::Shape>> io::readFileList(const QStringList &files)
{
    std::vector <std::pair<std::string,Types::Shape>> objlist;

    for (const auto &s : files)
    {
        
        QFileInfo finfo(s);
        
        if (!finfo.exists())
        {
            qWarning() << "file " << finfo.absoluteFilePath() << " does not exists";
            continue;
        }
        std::string fileobjname = (finfo.completeBaseName()+"."+finfo.completeSuffix()).toStdString();
        if (finfo.suffix().toLower() == "stl")
        {
            Types::Mesh obj = io::readstl(finfo.absoluteFilePath().toStdString());
            objlist.push_back({fileobjname,obj});
        }
        else if (finfo.suffix().toLower() == "ply")
        {
            std::vector <std::pair<std::string,Types::Shape>> shapes = io::readPly(finfo.absoluteFilePath().toStdString().c_str());
            for(auto& obj:shapes)
            {
                objlist.push_back({fileobjname+"/"+obj.first,obj.second});
            }

        }
        else if (finfo.suffix().toLower() == "obj")
        {
            std::vector <std::pair<std::string,Types::Shape>> shapes = io::readObj(finfo.absoluteFilePath().toStdString().c_str());
            for(auto& obj:shapes)
            {
                    objlist.push_back({fileobjname+"/"+obj.first,obj.second});
            }

        }
        else
        {
            qWarning() << "file type " << finfo.suffix() << " is unsupported";
        }
    }
    return objlist;
}