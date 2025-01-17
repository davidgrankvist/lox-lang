namespace CsLox.Cli.Errors;
internal class Reporter
{
    private readonly List<(int, int, string)> errors = [];

    public bool HasError => errors.Count > 0;

    public void Error(int line, int column, string message)
    {
        errors.Add((line, column, message));
    }

    public void Reset()
    {
        errors.Clear();
    }

    public void Output()
    {
        foreach (var (line, column, message) in errors) 
        {
            Console.Error.WriteLine($"Error at {line},{column} - {message}");
        }
        Reset();
    }
}
