param([string]$GameDirectory='', [string]$OutputDirectory='')
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
try {
 if(!$GameDirectory){$GameDirectory=Join-Path ${env:ProgramFiles(x86)} 'Steam/steamapps/common/Samorost 1'}
 if(!(Test-Path -LiteralPath (Join-Path $GameDirectory 'Data/Main.swf'))){
  Add-Type -AssemblyName System.Windows.Forms
  $dialog=New-Object System.Windows.Forms.FolderBrowserDialog
  $dialog.Description='Select your installed Samorost 1 folder'
  if($dialog.ShowDialog() -ne [System.Windows.Forms.DialogResult]::OK){throw 'Folder selection cancelled.'}
  $GameDirectory=$dialog.SelectedPath
 }
 $pythonFolder=Join-Path $root 'tools/player-python';$python=Join-Path $pythonFolder 'python.exe'
 $reused=$false
 foreach($candidate in @((Join-Path $env:LOCALAPPDATA 'Programs/Python/Python312-embed/python.exe'))){
  if(Test-Path -LiteralPath $candidate){
   & $candidate -c "import sys,numpy,PIL,cv2,lz4.block,importlib.metadata as m; assert sys.version_info[:2]==(3,12); assert {n:m.version(n) for n in ['numpy','pillow','opencv-python-headless','lz4']}=={'numpy':'1.26.4','pillow':'11.0.0','opencv-python-headless':'4.10.0.84','lz4':'4.3.3'}" 2>$null
   if($LASTEXITCODE -eq 0){$python=$candidate;$reused=$true;break}
  }
 }
 if(!(Test-Path -LiteralPath $python)){
  $downloads=Join-Path $root 'tools/downloads';New-Item -ItemType Directory -Force -Path $downloads | Out-Null
  $archive=Join-Path $downloads 'python-3.12.8-embed-amd64.zip';$expected='8d3f33be9eb810f23c102f08475af2854e50484b8e4e06275e937be61ce3d2fb'
  if(!(Test-Path -LiteralPath $archive) -or (Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash.ToLowerInvariant() -ne $expected){
   [Net.ServicePointManager]::SecurityProtocol=[Net.SecurityProtocolType]::Tls12
   Write-Host 'Preparing local setup tools (first run only)...'
   Invoke-WebRequest -UseBasicParsing -Uri 'https://www.python.org/ftp/python/3.12.8/python-3.12.8-embed-amd64.zip' -OutFile ($archive+'.part')
   if((Get-FileHash -LiteralPath ($archive+'.part') -Algorithm SHA256).Hash.ToLowerInvariant() -ne $expected){throw 'Python download checksum mismatch.'}
   Move-Item -LiteralPath ($archive+'.part') -Destination $archive -Force
  }
  if(Test-Path -LiteralPath $pythonFolder){throw 'Incomplete setup tools directory. Rename tools/player-python and retry.'}
  Expand-Archive -LiteralPath $archive -DestinationPath $pythonFolder
 }
 Push-Location $root
 try {
  if(!$reused){
   & $python -B scripts/dependencies.py --player
   if($LASTEXITCODE -ne 0){throw 'Could not prepare setup dependencies.'}
  }else{Write-Host 'Using existing Python conversion tools.'}
  $arguments=@('-u','-B','release/setup_player.py','--game',$GameDirectory)
  if($OutputDirectory){$arguments+=@('--output',$OutputDirectory)}
  & $python @arguments
  exit $LASTEXITCODE
 } finally {Pop-Location}
} catch {Write-Host ('ERROR: '+$_.Exception.Message);exit 1}