"use client";
import { api } from "@/lib/api";
import { Node } from "@/types";
import { createContext, useEffect, useState, ReactNode } from "react";

interface NodeContextType {
    nodeId: string | undefined;
    nodes: Node[];
    setNodeId: (id: string) => void;
}

export const NodeContext = createContext<NodeContextType | undefined>(
    undefined,
);

export function NodeProvider({ children }: { children: ReactNode }) {
    const [nodes, setNodes] = useState<Node[]>([]);
    const [nodeId, setNodeId] = useState<string | undefined>(undefined);

    useEffect(() => {
        const fetchNodes = async () => {
            try {
                const response = await api.get("/api/nodes");
                const data = Array.isArray(response.data) ? response.data : [];
                setNodes(data);
                
                // Set initial nodeId if not set
                if (data.length > 0 && !nodeId) {
                    setNodeId(data[0].id);
                }
            } catch (error) {
                console.error("Failed to fetch nodes:", error);
            }
        };

        fetchNodes();
        const interval = setInterval(fetchNodes, 5000);

        return () => clearInterval(interval);
    }, [nodeId]); // Keep nodeId to re-trigger if it's cleared, but interval handles updates

    return (
        <NodeContext.Provider value={{ nodeId, nodes, setNodeId }}>
            {children}
        </NodeContext.Provider>
    );
}
