#!/usr/bin/env python3

import time
import sys
import subprocess
import queue
import threading
import os
import re

Q = queue.Queue()

keeplooping=True

#pump the given stream and add data to queue
def pumper(so):
    while keeplooping:
        ln = ""
        while True:
            b = so.read(1)
            if len(b) == 0:
                return
            elif b == b"\n":
                break
            else:
                ln += b.decode(errors="replace")
        print(ln)
        sys.stdout.flush()
        Q.put(ln)
   

class Expectation:
    def __init__(self,a,b):
        self.shortname=a
        self.longname=b
        self.shortok=False
        self.longok=False


def main():
    make = os.getenv("MAKE_3701","make")
    
    now = time.time()
    t = now-int(now)
    t *= 1000
    t = int(t)
    t = "%03d" % t
    name1S = "A" + t + ".TXT"
    name1L = name1S
    name2L = "a" + t + "c def ghij klmn opq.txt"
    name2S = "A" + t + "CD~1.TXT"

    mfdat = open("Makefile").readlines()
    with open("thmf","w") as fp:
        for line in mfdat:
            fp.write(line)
            if line.upper().find("BILL.TXT") != -1 :
                tmp = line.replace("BILL.TXT",name1L)
                fp.write(tmp)
                tmp = line.replace("BILL.TXT",'"'+name2L+'"')
                fp.write(tmp)
                tmp = line.replace("BILL.TXT","XLA.ALX")
                fp.write(tmp)
                tmp = tmp.replace("cp","rm").replace("billofrights.txt","")
                fp.write(tmp)


    proc = subprocess.Popen(
        [make,"-f","thmf"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE)
        
    t1 = threading.Thread(target=pumper, args=(proc.stdout,))
    t1.start()
    t2 = threading.Thread(target=pumper, args=(proc.stderr,))
    t2.start()
    
    output = []
    
    startTime = time.time()
    while True:
        try:
            line = Q.get(block=True,timeout=10)
        except queue.Empty:
            print("Timed out!")
            proc.stdin.write(b"\x1b" b"c" b"quit\n")
            proc.stdin.flush()
            return
    
        output.append(line)
        if line == "DONE":
            break
        if time.time() - startTime > 10:
            print("Help!")
            proc.stdin.write(b"\x1b" b"c" b"quit\n")
            proc.stdin.flush()
            return
            
    
    print()
    print()
    
    proc.stdin.write(b"\x1b" b"c" b"quit\n")
    proc.stdin.flush()
    try:
        proc.wait(timeout=10)
    except subprocess.TimeoutExpired:
        print("You apparently aren't using -echr 27 in your Makefile")
        return
        
     
    expected = []
    expected.append(  Expectation("KERNEL.BIN","KERNEL.BIN") );
    for i in range(1,7):
        expected.append(  Expectation("ARTICLE"+str(i)+".TXT","article"+str(i)+".txt") );
        expected.append(  Expectation("BILL.TXT","BILL.TXT") );
        expected.append(  Expectation(name1S, name1L) );
        expected.append(  Expectation(name2S, name2L) );
        expected.append(  Expectation("MYFANC~1.TXT","my fancy filename.txt") );
        expected.append(  Expectation("THISIS~1.C","this is a really long filename.c") );

    outText = "\n".join(output)
    outText = outText.replace("\r\n","\n")
    
    si = outText.find("START");
    ei = outText.find("DONE");
    if si == -1 or ei == -1 :
        print("Missing START or DONE")
        return
    
    outText = outText[si+6 : ei ].strip()
    outLines = outText.split('\n')

    if outText.find("ALX") != -1:
        print("Found deleted file!")
        return

    gotTimes = True;
    timerex = re.compile(r"(\d{4})-(\d{2})-(\d{2}) (\d{1,2}):(\d{2}):(\d{2})");

    for line in outLines:
        ok=False
        for exp in expected:
            shortname = exp.shortname
            longname = exp.longname
            if line.find(shortname) != -1:
                if exp.shortok:
                    print("Duplicate short name: " + shortname)
                    return
                exp.shortok = True
                ok = True
            
            if line.find(longname) != -1:
                if exp.longok:
                    print("Duplicate long name: " + longname)
                    return
                exp.longok = True
                ok = True
        if ok:
            if gotTimes:
                M = timerex.search(line)
                if M:
                    year = int(M.group(1))
                    month = int(M.group(2))
                    day = int(M.group(3))
                    hour = int(M.group(4))
                    minute = int(M.group(5))
                    second = int(M.group(6))
                    T = time.mktime((
                        year, 
                        month,
                        day,
                        hour,
                        minute,
                        second,
                        0,
                        0,
                        -1
                    ))
                    timeDiff = abs( now - T )
                    if(timeDiff > 4):
                        print("Bad time on line " + line)
                        gotTimes = False
                else:
                    print("No creation time on line " + line)
                    gotTimes = False
        else:
            print("Extraneous line: " + line)
            return



    longNamesOK=True
    for exp in expected:
        if exp.longok == False:
            longNamesOK = False;
            print("No bonus: Missing long name "+exp.longname)
            break

    #if the long names are OK, we don't care about the short ones.
    if not longNamesOK:
        for exp in expected:
            if exp.shortok == False:
                print("Missing short name " + exp.shortname)
                return

    print()
    print()
    print()
    
    score = 100
    print(    "OK                      +100%")
    
    if(longNamesOK):
        print("Long name bonus:        + 40%")
        score += 40
    else:
        print("No long name bonus:     +  0%")

    if(gotTimes) :
        print("Creation time bonus:    + 25%")
        score += 20
    else:
        print("No creation time bonus: +  0%")

    print("Total score:             ",score, "%")


main()
