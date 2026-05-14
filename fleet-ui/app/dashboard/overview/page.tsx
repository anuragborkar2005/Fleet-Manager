"use client";

import { useContext } from "react";
import { KpiCards } from "@/components/overview/kpi-cards";
import { columns } from "@/components/command-center/columns";
import { NodeTable } from "@/components/overview/node_table";
import { NodeContext } from "@/providers/node-provider";

export default function OverviewPage() {
    const context = useContext(NodeContext);
    
    if (!context) {
        return <p>Loading context...</p>;
    }

    const { nodes } = context;

    return (
        <main className="w-full flex-1 items-center p-10">
            <section className="flex space-x-2 py-8">
                <KpiCards label="total nodes" value={nodes.length} />
                <KpiCards label="active nodes" value={nodes.filter(n => n.status === "online").length} />
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
                    {nodes.length === 0 ? (
                        <p>No nodes registered yet.</p>
                    ) : (
                        <NodeTable columns={columns} data={nodes} />
                    )}
                </div>
            </section>
        </main>
    );
}
