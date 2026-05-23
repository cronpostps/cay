@echo off
setlocal enabledelayedexpansion

:: ==========================================
:: 1. CẤU HÌNH PHIÊN BẢN (Sửa ở đây khi cập nhật)
:: ==========================================
set VERSION=v1.0.0

echo ===================================================
echo     CAY ENGINE - PUSH ^& RELEASE TO GITHUB
echo     Phien ban: %VERSION%
echo     Repo: cronpostps/cay
echo ===================================================

:: 2. Tự động lấy nội dung Commit hoặc nhập tay
set /p COMMIT_MSG="Nhap Commit (Enter de dung mac dinh): "
if "%COMMIT_MSG%"=="" set COMMIT_MSG=Release %VERSION% - Toi uu hoa code va fix bug

echo.
echo [1/3] Day ma nguon len Git...
git add .
git commit -m "%COMMIT_MSG%"
git push

echo.
echo [2/3] Dong goi file Release (.zip)...
set ZIP_NAME=CayEngine_%VERSION%.zip
if exist temp_release rmdir /s /q temp_release
mkdir temp_release
copy "build\Release\cay.exe" temp_release\ >nul
if exist "build\Release\cay_macros.txt" copy "build\Release\cay_macros.txt" temp_release\ >nul

powershell -Command "Compress-Archive -Path 'temp_release\*' -DestinationPath '%ZIP_NAME%' -Force"

echo.
echo [3/3] Up Release len GitHub...
gh release create %VERSION% "%ZIP_NAME%" --title "Cay Engine %VERSION%" --notes "%COMMIT_MSG%"

echo.
echo [4/4] Don dep hien truong...
rmdir /s /q temp_release
if exist "%ZIP_NAME%" del "%ZIP_NAME%"

echo.
echo ===================================================
echo [THANH CONG] Moi thu da len mây sach se!
echo ===================================================
pause