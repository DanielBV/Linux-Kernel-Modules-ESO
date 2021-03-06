
mapScancode = [("NULL","NULL"),("ESC","ESC"),("1","!"),("2","\""),("3","·"),("4","$"),("5","%"),("6","&"),("7","/"),("8","("),("9",")"),("0","=")
            ,("'","?"),("¡","¿"),("BACK","BACK"),("TAB","TAB"), ("q","Q"),("w","W"),("e","E"),("r","R"),("t","T"),("y","Y"),("u","U"),("i","I")
            ,("o","O"),("p","P"),("`","^"),("+","*"),(" \n","\n"), ("LCONTROL","LCONTROL"),("a","A"),("s","S"),("d","D"),("f","F"),("g","G")
            ,("h","H"),("j","J"),("k","K"),("l","L"),("ñ","Ñ"),("'","¨"),("º","ª"),("LSHIFT","LSHIFT"),("ç","Ç"),("z","Z"),("x","X"),("c","C")
            ,("v","V"),("b","B"),("n","N"),("m","M"),(",",";"),(".",":"),("-","_"),("RSHIFT","RSHIFT"),("*","*"), ("L-ALT","L-ALT"), (" "," ")
            , ("BlockMayus","BlockMayus")
    ]

shiftPressed = False

def toASCII(scancode:int):
    global mapScancode
    global shiftPressed
 
    if scancode in (99998,99999):
         # Right or left shift released
        shiftPressed = False
        return ""
    if scancode in (54,42):
        # Right or left shift pressed
        shiftPressed = True
        return ""

    if scancode<len(mapScancode) :
        return mapScancode[scancode][shiftPressed]
    else:
        return "UNKNOWN"

def main():
    """
    Reads one time the input of the user using the K-Input module
    """

    file = open("/dev/Kinput","r")
    test = file.read()
    
    for line in test.split():
            if line.strip() != "" :
                print(toASCII(int(line.strip())),end='')

       
                    
    file.close()

if __name__ == "__main__":
    main()
