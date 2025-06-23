import sys
import os

baseClassName = ''
baseClassPointer = ''
subClassNames = []
subClassFields = []
headers = ''

def getHeaders():
    global headers
    h = f"#pragma once\n\n"
    h += ("\n").join([header for header in headers.split(',')])
    h += f"\n#include <memory>\n\nclass {baseClassName};\n"
    for className in subClassNames:
        h += f"class {className};\n"
    h += f"\nusing {baseClassPointer} = std::unique_ptr<{baseClassName}>;\n"
    h += f"\ntemplate <typename T, typename... Args>\n"
    h += f"std::unique_ptr<T> make{baseClassName}(Args&&... args) {{\n"
    h += f"\treturn std::make_unique<T>(std::forward<Args>(args)...);\n}}\n\n"
    return h

def getVisitorInterface():
    interface = f"class {baseClassName}Visitor {{\npublic:\n"
    for className in subClassNames:
        interface += f"\tvirtual LiteralValue visit{className}({className}& {baseClassName.lower()}) = 0;\n"
    interface += f"\tvirtual ~{baseClassName}Visitor() = default;\n}};\n\n"
    return interface

def getBaseClass():
    baseClass = f"class {baseClassName} {{\npublic:\n"
    baseClass += f"\tvirtual LiteralValue accept({baseClassName}Visitor& visitor) = 0;\n"
    baseClass += f"\tvirtual ~{baseClassName}() = default;\n}};\n"
    return baseClass

def getSubClasses():
    subClasses = ''
    for i, className in enumerate(subClassNames):
        fields = [f.strip() for f in subClassFields[i] if f.strip()]
        subClasses += f"\nclass {className} : public {baseClassName} {{\npublic:\n"

        identifiers = []
        types = []

        for field in fields:
            parts = field.strip().split()
            type_part = ' '.join(parts[:-1])
            name = parts[-1]
            types.append(type_part)
            identifiers.append(name)
            clean_name = name[1:] if name.startswith('*') else name
            subClasses += f"\t{type_part} {clean_name};\n"

        subClasses += f"\n\t{className}("
        subClasses += ', '.join(
            f"{types[j]} {identifiers[j][1:]}" if identifiers[j].startswith('*')
            else f"{types[j]} {identifiers[j]}"
            for j in range(len(fields))
        )
        subClasses += ") : "
        subClasses += ', '.join(
            f"{identifiers[j][1:]}(std::move({identifiers[j][1:]}))" if identifiers[j].startswith('*')
            else f"{identifiers[j]}({identifiers[j]})"
            for j in range(len(fields))
        )
        subClasses += " {}\n"

        subClasses += f"\tLiteralValue accept({baseClassName}Visitor& visitor) override {{\n"
        subClasses += f"\t\treturn visitor.visit{className}(*this);\n\t}}\n}};\n"
    return subClasses

def generateAst(outputDir):
    if not os.path.exists(outputDir):
        os.makedirs(outputDir)

    path = os.path.join(outputDir, f"{baseClassName.lower()}.hpp")
    with open(path, 'w') as f:
        f.write(getHeaders())
        f.write(getVisitorInterface())
        f.write(getBaseClass())
        f.write(getSubClasses())

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: generateAst.py <class-info.txt> <output-directory>")
        exit(1)

    with open(sys.argv[1], 'r') as f:
        lines = [line.strip() for line in f.readlines() if line.strip()]
        headers = lines[0]
        baseClassName = lines[1]
        baseClassPointer = lines[2]
        lines = lines[3:]

        subClassNames = [line.split('->')[0].strip() for line in lines]
        subClassFields = [line.split('->')[1].split(',') for line in lines]

    generateAst(sys.argv[2])

