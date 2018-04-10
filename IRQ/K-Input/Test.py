
def toASCII(scancode:int):
    mapScancode = ["NULL","ESC","1","2","3","4","5","6","7","8","9","0","'","¡","BACK","TAB"
            , "Q","W","E","R","T","Y","U","I","O","P","`","+"," ENTER", "LCONTROL","A","S","D","F","G"
            ,"H","J","K","L","Ñ","'","º","LSHIF","ç","Z","X","C","V","B","N","M",",",".","-","RSHIFT","*"
            , "L-ALT", " ", "BlockMayus"
    ]
    if scancode<len(mapScancode) :
        return mapScancode[scancode]
    else:
        return "UNKNOWN"
def main():
    file = open("/dev/Kinput","r")
    
    
    test = file.read()
    print("TODO ES:" , test.split())
    for line in test.split():
           
            if line.strip() != "" :
                print(toASCII(int(line.strip())),end='')
                
    file.close()

if __name__ == "__main__":
    main()
