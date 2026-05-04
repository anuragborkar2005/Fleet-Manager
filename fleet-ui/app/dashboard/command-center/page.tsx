"use client";

import { useEffect, useState, useRef } from "react";
import { api } from "@/lib/api";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import {
    DeleteIcon,
    DollarSign,
    HistoryIcon,
    Keyboard,
    PlayIcon,
    SquareStopIcon,
} from "lucide-react";

interface HistoryItem {
    type: "command" | "output" | "error";
    content: string;
    nodeId?: string;
}

export default function CommandCenterPage() {
    const [nodes, setNodes] = useState<any[]>([]);
    const [selectedNodeId, setSelectedNodeId] = useState<string>("");
    const [input, setInput] = useState<string>("");
    const [history, setHistory] = useState<HistoryItem[]>([]);
    const [isExecuting, setIsExecuting] = useState(false);
    const scrollRef = useRef<HTMLDivElement>(null);

    useEffect(() => {
        const fetchNodes = async () => {
            try {
                const response = await api.get("/api/nodes");
                setNodes(response.data);
                if (response.data.length > 0 && !selectedNodeId) {
                    setSelectedNodeId(response.data[0].id);
                }
            } catch (error) {
                console.error("Failed to fetch nodes:", error);
            }
        };
        fetchNodes();
    }, []);

    useEffect(() => {
        if (scrollRef.current) {
            scrollRef.current.scrollTop = scrollRef.current.scrollHeight;
        }
    }, [history]);

    const handleExecute = async () => {
        if (!input.trim() || !selectedNodeId || isExecuting) return;

        const command = input.trim();
        setHistory(prev => [...prev, { type: "command", content: command, nodeId: selectedNodeId }]);
        setInput("");
        setIsExecuting(true);

        try {
            const response = await api.post(`/api/nodes/${selectedNodeId}/execute`, {
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
        } catch (error: any) {
            const errorMsg = error.response?.data?.error || error.message || "Execution failed";
            setHistory(prev => [...prev, { type: "error", content: errorMsg }]);
        } finally {
            setIsExecuting(false);
        }
    };

    const handleKeyDown = (e: React.KeyboardEvent) => {
        if (e.key === "Enter") {
            handleExecute();
        }
    };

    const clearLogs = () => setHistory([]);

    return (
        <div className="p-10 space-y-6">
            <div className="w-full max-w-7xl mx-auto flex flex-col space-y-4 shadow-md rounded-sm p-4 border border-slate-200 bg-white">
                <div className="w-full flex justify-between p-2">
                    <div className="flex items-center space-x-2">
                        <Keyboard className="size-4" />
                        <span className="text-xs font-bold text-slate-600 uppercase">
                            Command Input
                        </span>
                    </div>
                    <div className="flex space-x-2">
                        <select 
                            value={selectedNodeId} 
                            onChange={(e) => setSelectedNodeId(e.target.value)}
                            className="bg-slate-100 border rounded px-2 py-1 text-[10px] font-bold uppercase text-slate-700 outline-none"
                        >
                            {nodes.length === 0 && <option>No nodes online</option>}
                            {nodes.map(node => (
                                <option key={node.id} value={node.id}>
                                    Target: {node.hostname}
                                </option>
                            ))}
                        </select>
                        <div className="bg-slate-300 rounded-xs px-2 flex items-center">
                            <span className="text-slate-700 font-bold text-xs uppercase ">
                                secure mode
                            </span>
                        </div>
                    </div>
                </div>
                
                <div className="w-full flex items-center p-2">
                    <div className="w-full bg-slate-100 text-base transition-colors flex items-center rounded-md px-4 border border-slate-200">
                        <DollarSign className="size-4 text-emerald-600 mr-2" />
                        <Input 
                            className="block font-mono w-full rounded-none border-0 ring-0 outline-none bg-transparent focus-visible:ring-0 focus-visible:border-transparent placeholder:text-muted-foreground" 
                            placeholder="Enter shell command..."
                            value={input}
                            onChange={(e) => setInput(e.target.value)}
                            onKeyDown={handleKeyDown}
                            disabled={isExecuting}
                        />
                    </div>
                </div>

                <div className="flex space-x-3 items-center p-2">
                    <Button 
                        className="rounded-md" 
                        onClick={handleExecute}
                        disabled={isExecuting || !selectedNodeId}
                    >
                        <PlayIcon className="size-3 mr-2" />
                        <span>{isExecuting ? "Executing..." : "Execute"}</span>
                    </Button>
                    <Button variant="outline" className="rounded-md" onClick={clearLogs}>
                        <DeleteIcon className="size-3 mr-2" />
                        <span>Clear Log</span>
                    </Button>
                </div>
            </div>

            {/* Terminal Output */}
            <div className="w-full mx-auto bg-slate-950 min-h-[400px] max-w-7xl shadow-2xl rounded-lg flex flex-col border border-slate-800">
                <div className="flex items-center bg-slate-900 h-10 rounded-t-lg text-slate-400 px-4 border-b border-slate-800">
                    <div className="flex space-x-2 mr-4">
                        <div className="bg-rose-500/80 size-3 rounded-full" />
                        <div className="bg-amber-500/80 size-3 rounded-full" />
                        <div className="bg-emerald-500/80 size-3 rounded-full" />
                    </div>
                    <span className="font-mono text-xs uppercase tracking-widest">
                        Terminal Output {selectedNodeId && `— ${selectedNodeId}`}
                    </span>
                </div>

                <div 
                    ref={scrollRef}
                    className="flex-1 text-slate-300 font-mono p-4 space-y-2 overflow-y-auto text-sm"
                >
                    {history.length === 0 && (
                        <div className="text-slate-600 italic">No output to display. Run a command to see results.</div>
                    )}
                    {history.map((item, idx) => (
                        <div key={idx} className="whitespace-pre-wrap break-all">
                            {item.type === "command" ? (
                                <div className="flex items-start mt-2">
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
                    {isExecuting && (
                        <div className="flex items-center pl-8 text-slate-500 animate-pulse">
                            <span className="mr-2">Waiting for response...</span>
                        </div>
                    )}
                </div>
            </div>
        </div>
    );
}
