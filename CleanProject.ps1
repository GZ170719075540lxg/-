# Unreal Engine 项目清理脚本
# 用于解决 RebindPackages 警告和其他构建问题

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Unreal Engine 项目清理工具" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$ProjectRoot = Split-Path $PSScriptRoot -Parent

# 检查 Unreal Editor 是否正在运行
$EditorProcesses = Get-Process -Name "UnrealEditor" -ErrorAction SilentlyContinue
if ($EditorProcesses) {
    Write-Host "警告: 检测到 Unreal Editor 正在运行!" -ForegroundColor Yellow
    Write-Host "请先关闭 Unreal Editor，然后按任意键继续..." -ForegroundColor Yellow
    $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
}

Write-Host "开始清理项目..." -ForegroundColor Green
Write-Host ""

# 定义要删除的目录
$DirsToRemove = @(
    "Intermediate",
    "Binaries",
    "DerivedDataCache",
    "Saved/Config/CrashReportClient",
    "Saved/Logs",
    "Saved/Crashes"
)

# 删除目录
foreach ($dir in $DirsToRemove) {
    $fullPath = Join-Path $ProjectRoot $dir
    if (Test-Path $fullPath) {
        try {
            Write-Host "正在删除: $dir" -ForegroundColor Yellow
            Remove-Item -Recurse -Force $fullPath -ErrorAction Stop
            Write-Host "  ✓ 已删除" -ForegroundColor Green
        }
        catch {
            Write-Host "  ✗ 无法删除 (可能被占用): $dir" -ForegroundColor Red
            Write-Host "    错误: $($_.Exception.Message)" -ForegroundColor Red
        }
    }
    else {
        Write-Host "  ⊘ 不存在: $dir" -ForegroundColor Gray
    }
}

Write-Host ""
Write-Host "清理完成!" -ForegroundColor Green
Write-Host ""
Write-Host "下一步操作:" -ForegroundColor Cyan
Write-Host "1. 右键点击 Special_effects.uproject 文件" -ForegroundColor White
Write-Host "2. 选择 'Generate Visual Studio project files'" -ForegroundColor White
Write-Host "3. 打开生成的 .sln 文件并重新编译项目" -ForegroundColor White
Write-Host "4. 或者直接在 Epic Games Launcher 中启动项目" -ForegroundColor White
Write-Host ""
Write-Host "按任意键退出..." -ForegroundColor Gray
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

