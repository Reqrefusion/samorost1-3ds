param([Parameter(Mandatory=$true)][string]$Manifest)
$ErrorActionPreference='Stop'
Add-Type -AssemblyName PresentationCore
foreach($job in (Get-Content -LiteralPath $Manifest -Raw | ConvertFrom-Json)) {
 $stream=[IO.File]::OpenRead($job.source)
 try {
  $decoder=[Windows.Media.Imaging.BitmapDecoder]::Create($stream,[Windows.Media.Imaging.BitmapCreateOptions]::PreservePixelFormat,[Windows.Media.Imaging.BitmapCacheOption]::OnLoad)
  $frame=$decoder.Frames[0]
  if($frame.PixelWidth -ne $job.width -or $frame.PixelHeight -ne $job.height -or $frame.Format.ToString() -ne $job.format){throw ('Unexpected JPEG-XR dimensions or format: '+$job.source)}
  $stride=[int][Math]::Floor(($frame.PixelWidth*$frame.Format.BitsPerPixel+7)/8)
  $pixels=New-Object byte[] ($stride*$frame.PixelHeight)
  $frame.CopyPixels($pixels,$stride,0)
  [IO.File]::WriteAllBytes($job.target,$pixels)
 } finally {$stream.Close()}
}