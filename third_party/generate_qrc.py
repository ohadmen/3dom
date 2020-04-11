import sys
import ntpath
def main(argv):
    filename=argv[0]
    resource_name=argv[1]
    file_list=argv[2:]
    with open(filename, 'w') as fid:
        fid.write("<RCC>\n")
        fid.write("\t<qresource prefix=\"%s\">\n" % resource_name)
        for x in file_list:
            res_fn = (ntpath.basename(x).split('.')[0],x)
            fid.write("\t\t<file alias='%s' >%s</file>\n" % res_fn)
        fid.write("\t</qresource>\n")
        fid.write("</RCC>\n")
    fid.close()

if __name__=="__main__":
    main(sys.argv[1:])
