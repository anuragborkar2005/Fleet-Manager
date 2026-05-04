"use client";

import { ColumnDef } from "@tanstack/react-table";

export type Node = {
    hostname: string;
    ip: string;
    status: string;
    cpu_usage: number;
    ram_usage: number;
    uptime: number;
};

export const columns: ColumnDef<Node>[] = [
    {
        id: "select",
        header: ({ table }) => (
            <input
                type="checkbox"
                checked={table.getIsAllPageRowsSelected()}
                onChange={table.getToggleAllPageRowsSelectedHandler()}
            />
        ),
        cell: ({ row }) => (
            <input
                type="checkbox"
                checked={row.getIsSelected()}
                disabled={!row.getCanSelect()}
                onChange={row.getToggleSelectedHandler()}
            />
        ),
        enableSorting: false,
        enableHiding: false,
    },
    {
        accessorKey: "hostname",
        header: "Hostname",
    },
    {
        accessorKey: "ip",
        header: "IP Address",
    },
    {
        accessorKey: "status",
        header: "Status",
    },
    {
        accessorKey: "cpu_usage",
        header: "CPU Usage",
    },
    {
        accessorKey: "ram_usage",
        header: "RAM Usage",
    },
    {
        accessorKey: "uptime",
        header: "Uptime",
    },
];
