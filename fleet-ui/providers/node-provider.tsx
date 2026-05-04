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
                setNodes(response.data);
                if (response.data.length > 0 && !nodeId) {
                    setNodeId(response.data[0].id);
                }
            } catch (error) {
                console.error("Failed to fetch nodes:", error);
            }
        };
        fetchNodes();
    }, [nodeId]);

    return (
        <NodeContext.Provider value={{ nodeId, nodes, setNodeId }}>
            {children}
        </NodeContext.Provider>
    );
}
