using CsLox.Cli.Scanning;

internal class Program
{
    static void Main(string[] args)
    {
        var lox = new Lox();
        if (args.Length >= 1)
        {
            var filePath = args[0];
            var program = File.ReadAllText(filePath);
            lox.Run(program);
        }
        else
        {
            string? line;
            while ((line = Console.ReadLine()) != null)
            {
                var rawLine = line + Environment.NewLine;
                lox.Run(rawLine);
            }
        }
    }
}