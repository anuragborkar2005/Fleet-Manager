"use client";
import { useState, useRef, useEffect } from "react";
import { api } from "@/lib/api";

interface ShellProps {
    title: string;
    nodeId: string;
}

interface HistoryItem {
    type: "command" | "output" | "error";
    content: string;
}

export function Shell({ title, nodeId }: ShellProps) {
    const [history, setHistory] = useState<HistoryItem[]>([]);
    const [input, setInput] = useState<string>("");
    const [isExecuting, setIsExecuting] = useState(false);
    const scrollRef = useRef<HTMLDivElement>(null);

    useEffect(() => {
        if (scrollRef.current) {
            scrollRef.current.scrollTop = scrollRef.current.scrollHeight;
        }
    }, [history]);

    const handleKeyDown = async (e: React.KeyboardEvent<HTMLTextAreaElement>) => {
        if (e.key === "Enter" && !e.shiftKey) {
            e.preventDefault();
            if (input.trim() !== "" && !isExecuting) {
                const command = input.trim();
                setHistory(prev => [...prev, { type: "command", content: command }]);
                setInput("");
                setIsExecuting(true);

                try {
                    const response = await api.post(`/api/nodes/${nodeId}/execute`, {
                        command: command
                    });

                    if (response.data.stdout) {
                        setHistory(prev => [...prev, { type: "output", content: response.data.stdout }]);
                    }
                    if (response.data.stderr) {
                        setHistory(prev => [...prev, { type: "error", content: response.data.stderr }]);
                    }
                    if (!response.data.stdout && !response.data.stderr) {
                      setHistory(prev => [...prev, { type: "output", content: "(No output)" }]);
                    }
                } catch (error: unknown) {
                    let errorMsg = "Execution failed";
                    if (error instanceof Error) {
                        errorMsg = error.message;
                    }
                    if (typeof error === 'object' && error !== null && 'response' in error) {
                        const axiosError = error as { response: { data: { error?: string } } };
                        if (axiosError.response?.data?.error) {
                            errorMsg = axiosError.response.data.error;
                        }
                    }
                    setHistory(prev => [...prev, { type: "error", content: errorMsg }]);
                } finally {
                    setIsExecuting(false);
                }
            }
        }
    };

    return (
        <div className="w-full h-[500px] mx-auto bg-slate-950 max-w-7xl shadow-2xl rounded-lg flex flex-col border border-slate-800">
            {/* Header */}
            <div className="flex items-center bg-slate-900 h-10 rounded-t-lg text-slate-400 px-4 border-b border-slate-800">
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
            <div 
                ref={scrollRef}
                className="flex-1 text-slate-300 font-mono p-4 space-y-2 overflow-y-auto text-sm"
            >
                <div className="text-slate-500 italic mb-4">
                    Session established. Type a command and press Enter.
                </div>
                {history.map((item, idx) => (
                    <div key={idx} className="whitespace-pre-wrap break-all">
                        {item.type === "command" ? (
                            <div className="flex items-start">
                                <span className="text-emerald-500 font-bold mr-2">➜</span>
                                <span className="text-white font-bold">{item.content}</span>
                            </div>
                        ) : (
                            <div className={`pl-5 ${item.type === "error" ? "text-rose-400" : "text-slate-300"}`}>
                                {item.content}
                            </div>
                        )}
                    </div>
                ))}
                {isExecuting && (
                    <div className="flex items-center pl-5 text-slate-500 animate-pulse">
                        <span className="mr-2">Executing...</span>
                    </div>
                )}
            </div>

            {/* Input area */}
            <div className="bg-slate-900/50 p-3 border-t border-slate-800">
                <div className="flex items-start">
                    <span className="text-emerald-500 font-bold mr-2 mt-1">➜</span>
                    <textarea
                        className="flex-1 resize-none bg-transparent text-white font-mono border-0 outline-none ring-0 focus:ring-0 text-sm py-1"
                        rows={1}
                        value={input}
                        onChange={(e) => setInput(e.target.value)}
                        onKeyDown={handleKeyDown}
                        placeholder={isExecuting ? "Wait for command to finish..." : "Type command..."}
                        disabled={isExecuting}
                    />
                </div>
            </div>
        </div>
    );
}
