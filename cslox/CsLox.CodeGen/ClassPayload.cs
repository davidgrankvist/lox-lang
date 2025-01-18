namespace CsLox.CodeGen;
internal class ClassPayload
{
    private const string ExprType = "Expr";
    private const string StmtType = "Stmt";

    public ClassPayload(ClassPayloadType type, string name, List<string> properties)
    {
        Name = name; 
        Type = type;
        Properties = properties;
        TypeStr = ToTypeStr(type);
    }

    public string Name { get; }

    public ClassPayloadType Type { get; }

    public List<string> Properties { get; }

    public string TypeStr { get; }

    private static string ToTypeStr(ClassPayloadType type)
    {
        switch (type)
        {
            case ClassPayloadType.Expression:
                return ExprType;
            case ClassPayloadType.Statement:
                return StmtType;
            default:
                throw new ArgumentOutOfRangeException("Unsupported class payload type");
        }
    }
}
