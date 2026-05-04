"use client";

import React from "react";
import { TooltipProvider } from "@/components/ui/tooltip";
import { NodeProvider } from "./node-provider";

export default function AppProvider({
    children,
}: {
    children: React.ReactNode;
}) {
    return (
        <TooltipProvider>
            <NodeProvider>{children}</NodeProvider>
        </TooltipProvider>
    );
}
