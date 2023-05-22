<#
#̷𝓍   𝓐𝓡𝓢 𝓢𝓒𝓡𝓘𝓟𝓣𝓤𝓜
#̷𝓍   🇵​​​​​🇴​​​​​🇼​​​​​🇪​​​​​🇷​​​​​🇸​​​​​🇭​​​​​🇪​​​​​🇱​​​​​🇱​​​​​ 🇸​​​​​🇨​​​​​🇷​​​​​🇮​​​​​🇵​​​​​🇹​​​​​ 🇧​​​​​🇾​​​​​ 🇬​​​​​🇺​​​​​🇮​​​​​🇱​​​​​🇱​​​​​🇦​​​​​🇺​​​​​🇲​​​​​🇪​​​​​🇵​​​​​🇱​​​​​🇦​​​​​🇳​​​​​🇹​​​​​🇪​​​​​.🇶​​​​​🇨​​​​​@🇬​​​​​🇲​​​​​🇦​​​​​🇮​​​​​🇱​​​​​.🇨​​​​​🇴​​​​​🇲​​​​​
#>


Function Get-ExternalLibrariesPackage{
    [CmdletBinding(SupportsShouldProcess)]
    param()
    try{
       $DataFilePath = (Resolve-Path "$PSScriptRoot\..").Path

       $Url = "https://drive.google.com/uc?id=1bUxk8kNsKsQ-NQnl7hu4ONKwvvk4_E2X&export=download&confirm=t&uuid=39d9390f-1989-47be-a910-9e7b5312c700&at=AKKF8vwo-fWvw-8lwc2szBEriu6H:1684709996960"
       $DataFile = Join-Path $DataFilePath "Libs.zip"
       $MyHeaders = @{
        "authority"="drive.google.com"
        "method"="POST"
        "path"="/uc?id=1bUxk8kNsKsQ-NQnl7hu4ONKwvvk4_E2X&export=download&confirm=t&uuid=39d9390f-1989-47be-a910-9e7b5312c700&at=AKKF8vwo-fWvw-8lwc2szBEriu6H:1684709996960"
        "scheme"="https"
        "accept"="text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9"
        "accept-encoding"="gzip, deflate, br"
        "accept-language"="fr-FR,fr;q=0.9,en-US;q=0.8,en;q=0.7"
        "cache-control"="max-age=0"
        "origin"="https://drive.google.com"
        "referer"="https://drive.google.com/uc?id=1bUxk8kNsKsQ-NQnl7hu4ONKwvvk4_E2X&export=download"
        "upgrade-insecure-requests"="1"
        "x-chrome-connected"="source=Chrome,id=111603568843057468854,mode=0,enable_account_consistency=false,consistency_enabled_by_default=false"
        "x-client-data"="CI+2yQEIo7bJAQipncoBCNnTygEIm4DLAQiUocsBCIurzAE="
      }
      $session = New-Object Microsoft.PowerShell.Commands.WebRequestSession
      $session.UserAgent = "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/104.0.0.0 Mobile Safari/537.36"

      Write-Host "Downloading External Libraries Package to `"$DataFile`"...`n`n"
      
      Invoke-WebRequest -UseBasicParsing -Uri $Url -Method "POST" -WebSession $session -Headers $MyHeaders -ContentType "application/x-www-form-urlencoded" -OutFile "$DataFile"

      return $DataFile
    }catch{
      Write-Error "$_"
    }
}

Function Install-ExternalLibrariesPackage{
    [CmdletBinding(SupportsShouldProcess)]
        param(
        [parameter(mandatory=$true, Position=0)]
        [String]$Path
        )

    try{
        $RootDirectory = (Resolve-Path "$PSScriptRoot\..").Path
      
        $FileInfo = Get-Item $Path 
        $FileDirectory = $FileInfo.DirectoryName
        $ExtractedFolder = Join-Path $RootDirectory "Recon-AgentManager_ExternalLibs"

        Push-Location $FileDirectory

        Write-Host "Clearing `"$ExtractedFolder`"..."
        Remove-Item -Path "$ExtractedFolder" -Recurse  -Force -ErrorAction Ignore | Out-Null

        Write-Host "Extracting Archive."
        Expand-Archive -Path $Path -DestinationPath $FileDirectory -Force

        if(!(Test-Path $ExtractedFolder)){ throw "invalid extracted folder"}

        $Sources = @("$ExtractedFolder\MyCrypto\Lib","$ExtractedFolder\MyHpSocket\Lib","$ExtractedFolder\MyMsgPack\Lib")
        $Destination = @("$RootDirectory\MyCrypto\Lib","$RootDirectory\MyHpSocket\Lib","$RootDirectory\MyMsgPack\Lib")

        for($i = 0 ; $i -lt 3 ; $i++){
          $dest = $Destination[$i]
          $src = $Sources[$i]
          Write-Host "Processing `"$dest`"..."
          Remove-Item -Path "$dest" -Recurse  -Force -ErrorAction Ignore | Out-Null
          Move-Item $src $dest -Force -ErrorAction Ignore
        }
        Write-Host "Clearing `"$ExtractedFolder`"..."
        Remove-Item -Path "$ExtractedFolder" -Recurse  -Force -ErrorAction Ignore | Out-Null
    }catch{
      Write-Error "$_"
    }
}


$DataFilePath = Get-ExternalLibrariesPackage
if($DataFilePath -ne $Null){
  Install-ExternalLibrariesPackage -Path $DataFilePath
}
      