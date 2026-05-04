import { Card, CardContent } from "../ui/card";

export function KpiCards({ label, value }: { label: string; value: number }) {
    return (
        <Card className="w-full max-w-sm border border-gray-200 shadow-2xs rounded-md">
            <CardContent className="flex flex-col gap-2">
                <span className="text-xs font-medium  tracking-wide">
                    {label.toUpperCase()}
                </span>
                <span className="text-2xl font-semibold ">{value}</span>
            </CardContent>
        </Card>
    );
}
