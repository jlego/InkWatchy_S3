DEP = "/Users/ronghuiyu/.platformio/penv/lib/python3.10/site-packages/platformio/dependencies.py"

with open(DEP, "r") as f:
    content = f.read()

old = 'pioarduino/scons/releases/\n            "download/4.11.1/scons-local-4.11.1.tar.gz"'
new = 'SCons/scons/releases/\n            "download/4.6.0/scons-local-4.6.0.tar.gz"'

if old in content:
    content = content.replace(old, new)
    with open(DEP, "w") as f:
        f.write(content)
    print("DONE - dependencies.py updated")
else:
    print("OLD PATTERN NOT FOUND, showing relevant lines:")
    for i, line in enumerate(content.split("\n"), 1):
        if "scons" in line.lower():
            print(f"  {i}: {line}")