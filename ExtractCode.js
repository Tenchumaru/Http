var fso = WScript.CreateObject("Scripting.FileSystemObject");

function Main() {
  var fin = WScript.Arguments.length > 0 ? fso.OpenTextFile(WScript.Arguments(0), 1, false, 0) : WScript.StdIn;
  var fout = WScript.Arguments.length > 1 ? fso.CreateTextFile(WScript.Arguments(1)) : WScript.StdOut;

  // Skip the first line.  The implementation prints it.
  fin.ReadLine();
  while (!fin.AtEndOfStream) {
    var text = fin.ReadLine();
    fout.WriteLine('\t*pout << "' + text.replace(/\\/g, "\\\\").replace(/"/g, '\\"') + '" << std::endl;');
  }
}

Main();
