export default function ShellPage() {
    return (
        <div className="p-10 flex flex-col items-center justify-center min-h-[60vh] text-center space-y-4">
            <h1 className="text-4xl font-bold">Shell Page Relocated</h1>
            <p className="text-muted-foreground max-w-md">
                Interactive shell functionality has been moved to the 
                <a href="/dashboard/command-center" className="text-primary font-bold underline ml-1">Command Center</a>.
            </p>
        </div>
    );
}
