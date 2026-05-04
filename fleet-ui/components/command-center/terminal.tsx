"use client";

interface TerminalProps {
    title: string;
    history?: { type: "command" | "output" | "error"; content: string; nodeId?: string }[];
}

export function Terminal({ title, history = [] }: TerminalProps) {
    return (
        <div className="w-full mx-auto bg-slate-950 min-h-96 max-w-7xl shadow-2xl rounded-lg flex flex-col border border-slate-800">
            {/* Header */}
            <div className="flex items-center bg-slate-900 h-12 rounded-t-lg text-slate-400 px-4 border-b border-slate-800">
                <div className="flex space-x-2 mr-4">
                    <div className="bg-rose-500/80 size-3 rounded-full" />
                    <div className="bg-amber-500/80 size-3 rounded-full" />
                    <div className="bg-emerald-500/80 size-3 rounded-full" />
                </div>
                <span className="font-mono text-xs uppercase tracking-widest">
                    {title}
                </span>
            </div>

            {/* Body */}
            <div className="flex-1 text-slate-300 font-mono p-4 space-y-2 overflow-y-auto text-sm">
                {history.length === 0 && (
                    <div className="text-slate-600 italic">No output to display.</div>
                )}
                {history.map((item, idx) => (
                    <div key={idx} className="whitespace-pre-wrap break-all">
                        {item.type === "command" ? (
                            <div className="flex items-start">
                                <span className="text-emerald-500 font-bold mr-2">[{item.nodeId}] $</span>
                                <span className="text-white font-bold">{item.content}</span>
                            </div>
                        ) : (
                            <div className={`pl-8 ${item.type === "error" ? "text-rose-400" : "text-slate-300"}`}>
                                {item.content}
                            </div>
                        )}
                    </div>
                ))}
            </div>
        </div>
    );
}
