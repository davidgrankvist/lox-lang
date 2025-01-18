using CsLox.CodeGen.Generation;

namespace CsLox.CodeGen;

internal class Program
{
    static void Main(string[] args)
    {
        List<ClassPayload> payloads = GrammarConstants.Grammar;
        var files = Generator.Generate(payloads, "CsLox.Cli.Parsing.Generated");

        var currentDir = AppDomain.CurrentDomain.BaseDirectory;
        var targetDir = Path.Combine(currentDir, "..", "..", "..", "..", "CsLox.Cli", "Parsing", "Generated");

        foreach (var file in files)
        {
            var targetPath = Path.Combine(targetDir, file.Name);
            File.WriteAllText(targetPath, file.Content);
        }
    }
}
