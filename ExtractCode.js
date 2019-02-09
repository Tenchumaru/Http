var fso = WScript.CreateObject("Scripting.FileSystemObject");

function Main() {
  var fin = WScript.Arguments.length > 0 ? fso.OpenTextFile(WScript.Arguments(0), 1, false, 0) : WScript.StdIn;
  var fout = WScript.Arguments.length > 1 ? fso.CreateTextFile(WScript.Arguments(1)) : WScript.StdOut;

  var text;
  do {
    text = fin.ReadLine();
  } while (!~text.indexOf("// <<<"));
  for (; text = fin.ReadLine(), !~text.indexOf("// >>>");) {
    fout.WriteLine('\t*pout << "' + text.replace(/\\/g, "\\\\").replace(/"/g, '\\"') + '" << std::endl;');
  }
}

Main();
