namespace CsLox.CodeGen.Generation;
internal readonly struct FileData
{
    public FileData(string name, string content)
    {
        Name = name;
        Content = content;
    }

    public readonly string Name;

    public readonly string Content;
}
