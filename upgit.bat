@echo off
setlocal enabledelayedexpansion

:: ==========================================
:: 1. CẤU HÌNH (Sửa VERSION ở đây khi cập nhật)
:: ==========================================
set VERSION=v1.0.1
set REPO=cronpostps/Chanh

echo ===================================================
echo     CHANH ENGINE - PUSH ^& ROLLING RELEASE
echo     Phien ban hien tai: %VERSION%
echo     Target: github.com/%REPO%/releases/tag/latest
echo ===================================================

:: 2. Nhập nội dung Commit
set /p COMMIT_MSG="Nhap Commit (Enter de dung mac dinh): "
if "%COMMIT_MSG%"=="" set COMMIT_MSG=Release %VERSION% - Toi uu hoa code va fix bug

echo.
echo [1/3] Day ma nguon len Git...
git add .
git commit -m "%COMMIT_MSG%"
git push

echo.
echo [2/3] Dong goi file Release (.zip)...
:: Đặt tên file ZIP cố định để link tải luôn là một
set ZIP_NAME=ChanhEngine_latest.zip
if exist temp_release rmdir /s /q temp_release
mkdir temp_release

:: === ĐÃ ĐỔI TÊN THÀNH CHANH ===
copy "build\Release\chanh.exe" temp_release\ >nul
if exist "build\Release\chanh_macros.txt" copy "build\Release\chanh_macros.txt" temp_release\ >nul
if exist "build\Release\chanh_exclude.txt" copy "build\Release\chanh_exclude.txt" temp_release\ >nul

powershell -Command "Compress-Archive -Path 'temp_release\*' -DestinationPath '%ZIP_NAME%' -Force"

echo.
echo [3/3] Up Release vao tag 'latest'...
:: Xóa bản release 'latest' cũ trên GitHub
gh release delete latest --cleanup-tag -y --repo %REPO% >nul 2>&1

:: Tạo release 'latest' mới
gh release create latest "%ZIP_NAME%" --repo %REPO% --title "Chanh Engine %VERSION%" --notes "%COMMIT_MSG%" --latest

:: Chốt chặn an toàn
if %errorlevel% neq 0 (
    echo.
    echo [LOI] Up Release that bai! Vui long doc thong bao loi cua GitHub o tren.
    echo File zip van an toan nam o thu muc nay: %ZIP_NAME%
    rmdir /s /q temp_release
    pause
    exit /b
)

echo.
echo [4/4] Don dep hien truong...
rmdir /s /q temp_release
if exist "%ZIP_NAME%" del "%ZIP_NAME%"

echo.
echo ===================================================
echo [THANH CONG] Moi thu da len mây sach se!
echo Link tai truc tiep luon luon la:
echo https://github.com/%REPO%/releases/download/latest/%ZIP_NAME%
echo ===================================================
pause