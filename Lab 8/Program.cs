using System;
using System.IO;
using System.Threading;
using System.Diagnostics;
using System.Collections.Generic;

namespace testharness
{
    class MainClass
    {
        public static void Main(string[] args)
        {
            var make = System.Environment.GetEnvironmentVariable("MAKE_3701");
            if(make == null)
                make = "make";

            var proc = new Process();
            proc.StartInfo.FileName = make;
            proc.StartInfo.RedirectStandardInput = true;
            proc.StartInfo.RedirectStandardOutput = true;
            proc.StartInfo.RedirectStandardError = true;
            proc.StartInfo.UseShellExecute = false;
            proc.Start();
            var output = new List<string>();

            Action Done = (() => {
                proc.StandardInput.Write("\x1b");
                proc.StandardInput.Write("c");
                proc.StandardInput.Write("quit\n");
                proc.StandardInput.Flush();
            });

            System.Diagnostics.DataReceivedEventHandler eventHandler = ((sender, e) => {
                string s = e.Data;
                if(s != null ){
                    output.Add(s);
                    Console.WriteLine(s);
                    if(s.IndexOf("SOUR") != -1)
                        Done();
                }
            });
            proc.OutputDataReceived += eventHandler;
            proc.ErrorDataReceived += eventHandler;
            proc.BeginOutputReadLine();
            proc.BeginErrorReadLine();
            proc.WaitForExit(5000); 

            var expectedOutput = @"SWEET!
Bytes per sector: 512
Sectors per cluster: 8
Root cluster: 2
OEM: ETEC3701
Label: MOOCOW12345
SOUR";

            var actualOutput = String.Join("\n", output);

            if(actualOutput.IndexOf(expectedOutput) != -1)
                Console.WriteLine("OK");
            else
                Console.WriteLine("NOT FOUND");

                
        }
    }
}
