namespace CsLox.CodeGen.Generation;
internal class Generator
{
    private const string Indent = "    ";
    private const string Generic = "TResult";

    public static List<FileData> Generate(List<ClassPayload> payloads, string nameSpace)
    {
        var expressions = payloads.Where(p => p.Type == ClassPayloadType.Expression).ToArray();
        var statements = payloads.Where(p => p.Type == ClassPayloadType.Statement).ToArray();

        var expressionFile = Process(expressions, nameSpace);
        var statementFile = Process(statements, nameSpace);

        List<FileData?> result = [expressionFile, statementFile];

        return result.Where(f => f != null).Select(f => f!.Value).ToList();
    }

    private static FileData? Process(ClassPayload[] payloads, string nameSpace)
    {
        if (payloads.Length == 0)
        {
            return null;
        }

        var type = payloads[0].TypeStr;
        var content = GenerateAstClass(payloads, nameSpace);

        return new FileData($"{type}.cs", content);
    }

    private static string GenerateAstClass(ClassPayload[] payloads, string nameSpace)
    {
        var type = payloads[0].TypeStr;

        var result = "// DO NOT EDIT - THIS CLASS IS AUTO-GENERATED" + Environment.NewLine;
        result += "// See the CsLox.CodeGen project" + Environment.NewLine + Environment.NewLine;
        result += "using CsLox.Cli.Scanning;" + Environment.NewLine + Environment.NewLine;
        result += $"namespace {nameSpace};" + Environment.NewLine + Environment.NewLine;
        result += $"public abstract class {type}" + Environment.NewLine;
        result += "{" + Environment.NewLine;
        result += GenerateVisitorInterface(payloads, Indent);
        result += Indent + $"public abstract {Generic} Accept<{Generic}>(IVisitor<{Generic}> visitor);" + Environment.NewLine + Environment.NewLine;
        result += GenerateAstSubClasses(payloads, Indent);
        result += Environment.NewLine + "}";

        return result;
    }

    private static string GenerateVisitorInterface(ClassPayload[] payloads, string indent)
    {
        var result = indent + $"public interface IVisitor<{Generic}>" + Environment.NewLine;
        result += indent + "{" + Environment.NewLine;
        foreach (var payload in payloads)
        {
            var nameType = payload.Name + payload.TypeStr;
            result += indent + indent + $"{Generic} Visit{nameType}({nameType} {payload.TypeStr.ToLower()});" + Environment.NewLine;
        }
        result += indent + "}" + Environment.NewLine + Environment.NewLine;

        return result;
    }

    private static string GenerateAstSubClasses(ClassPayload[] payloads, string indent)
    {
        var result = "";
        foreach (var payload in payloads)
        {
            var nameType = payload.Name + payload.TypeStr;
            result += indent + $"public class {nameType} : {payload.TypeStr}" + Environment.NewLine;
            result += indent + "{" + Environment.NewLine;

            result += GenerateConstructor(payload, indent + Indent);
            result += GenerateFields(payload, indent + Indent);
            result += GenerateAcceptImplementation(payload, indent + Indent);

            result += indent + "}" + Environment.NewLine;
        }
        return result;
    }

    private static string GenerateConstructor(ClassPayload payload, string indent)
    {
        var result = "";

        var nameType = payload.Name + payload.TypeStr;
        var ctorArgs = string.Join(", ", payload.Properties);
        result += indent + $"public {nameType}({ctorArgs})" + Environment.NewLine;
        result += indent + "{" + Environment.NewLine;
        foreach (var prop in payload.Properties)
        {
            var propName = prop.Split()[1];
            result += indent + Indent + $"this.{propName} = {propName};" + Environment.NewLine;
        }
        result += indent + "}" + Environment.NewLine + Environment.NewLine;

        return result;
    }

    private static string GenerateFields(ClassPayload payload, string indent)
    {
        var result = "";

        foreach (var prop in payload.Properties)
        {
            result += indent + $"public readonly {prop};" + Environment.NewLine;
        }
        result += Environment.NewLine;

        return result;
    }

    private static string GenerateAcceptImplementation(ClassPayload payload, string indent)
    {
        var result = "";

        var nameType = payload.Name + payload.TypeStr;
        result += indent + $"public override {Generic} Accept<{Generic}>(IVisitor<{Generic}> visitor)" + Environment.NewLine;
        result += indent + "{" + Environment.NewLine;
        result += indent + Indent + $"return visitor.Visit{nameType}(this);" + Environment.NewLine;
        result += indent + "}" + Environment.NewLine;

        return result;
    }

    // Example for reference 
    //
    //public abstract class Expr
    //{
    //    public interface IVisitor<TResult>
    //    {
    //        TResult VisitLiteralExpr(LiteralExpr expr);
    //    }

    //    protected abstract TResult Accept<TResult>(IVisitor<TResult> visitor);

    //    public class LiteralExpr : Expr
    //    {
    //        public LiteralExpr(object value)
    //        {
    //            this.value = value;
    //        }

    //        public readonly object value;

    //        protected override TResult Accept<TResult>(IVisitor<TResult> visitor)
    //        {
    //            return visitor.VisitLiteralExpr(this);
    //        }
    //    }
    //}
}
