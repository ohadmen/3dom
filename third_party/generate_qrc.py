import sys


def main(argv):
    filename=argv[0]
    file_list=argv[1:]
    with open(filename, 'w') as fid:
        fid.write("<RCC>\n")
        fid.write("\t<qresource prefix=\"/%s\">\n" % filename[:-3])
        for x in file_list:
            fid.write("\t\t<file>%s</file>\n" % x)
        fid.write("\t</qresource>\n")
        fid.write("</RCC>\n")
    fid.close()

if __name__=="__main__":
    main(sys.argv[1:])
