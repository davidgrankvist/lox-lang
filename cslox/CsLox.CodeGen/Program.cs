namespace CsLox.CodeGen;

internal class Program
{
    static void Main(string[] args)
    {
        List<ClassPayload> payloads = [
            // expressions
            new ClassPayload(ClassPayloadType.Expression, "Literal", ["object Value"]),
            new ClassPayload(ClassPayloadType.Expression, "Group", ["Expr Expression"]),
            new ClassPayload(ClassPayloadType.Expression, "Unary", ["Token Operator", "Expr Expression"]),
            new ClassPayload(ClassPayloadType.Expression, "Binary", ["Expr Left", "Token Operator", "Expr Right"]),
            // statements
            new ClassPayload(ClassPayloadType.Statement, "Expression", ["Expr Expression"]),
            new ClassPayload(ClassPayloadType.Statement, "Program", ["List<Stmt> Statements"]),
            new ClassPayload(ClassPayloadType.Statement, "Print", ["Expr Expression"]),
        ];

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
