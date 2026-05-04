"use client";

import { useEffect, useState } from "react";
import { KpiCards } from "@/components/overview/kpi-cards";
import { columns } from "@/components/command-center/columns";
import { NodeTable } from "@/components/overview/node_table";
import { api } from "@/lib/api";
import { Node } from "@/types";

export default function OverviewPage() {
    const [nodes, setNodes] = useState([]);
    const [loading, setLoading] = useState(true);

    useEffect(() => {
        const fetchNodes = async () => {
            try {
                const response = await api.get("/api/nodes");
                const mappedNodes = response.data.map((node: Node) => ({
                    hostname: node.hostname,
                    ip: node.ip,
                    status: node.status,
                    cpu_usage: 0,
                    ram_usage: 0,
                    uptime: 0,
                }));
                setNodes(mappedNodes);
            } catch (error) {
                console.error("Failed to fetch nodes:", error);
            } finally {
                setLoading(false);
            }
        };

        fetchNodes();
        const interval = setInterval(fetchNodes, 5000); // Poll every 5 seconds
        return () => clearInterval(interval);
    }, []);

    return (
        <main className="w-full flex-1 items-center p-10">
            <section className="flex space-x-2 py-8">
                <KpiCards label="total active nodes" value={nodes.length} />
                <KpiCards label="fleet cpu usage" value={0} />
                <KpiCards label="active incident" value={0} />
                <KpiCards label="fleet uptime" value={0} />
            </section>
            <section>
                <div>
                    <span className="text-xl font-semibold text-primary-background">
                        Node Overview
                    </span>
                </div>
                <div className="py-8">
                    {loading && nodes.length === 0 ? (
                        <p>Loading nodes...</p>
                    ) : (
                        <NodeTable columns={columns} data={nodes} />
                    )}
                </div>
            </section>
        </main>
    );
}
