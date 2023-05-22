<#
#̷𝓍   𝓐𝓡𝓢 𝓢𝓒𝓡𝓘𝓟𝓣𝓤𝓜
#̷𝓍   🇵​​​​​🇴​​​​​🇼​​​​​🇪​​​​​🇷​​​​​🇸​​​​​🇭​​​​​🇪​​​​​🇱​​​​​🇱​​​​​ 🇸​​​​​🇨​​​​​🇷​​​​​🇮​​​​​🇵​​​​​🇹​​​​​ 🇧​​​​​🇾​​​​​ 🇬​​​​​🇺​​​​​🇮​​​​​🇱​​​​​🇱​​​​​🇦​​​​​🇺​​​​​🇲​​​​​🇪​​​​​🇵​​​​​🇱​​​​​🇦​​​​​🇳​​​​​🇹​​​​​🇪​​​​​.🇶​​​​​🇨​​​​​@🇬​​​​​🇲​​​​​🇦​​​​​🇮​​​​​🇱​​​​​.🇨​​​​​🇴​​​​​🇲​​​​​
#>


function Invoke-IsAdministrator  {  
    (New-Object Security.Principal.WindowsPrincipal ([Security.Principal.WindowsIdentity]::GetCurrent())).IsInRole([Security.Principal.WindowsBuiltinRole]::Administrator)  
}
function Get-ScriptDirectory {
    Split-Path -Parent $PSCommandPath
}

      try{

        [string]$ErrorDetails=''
        [System.Boolean]$ErrorOccured=$False

        ########################################################################################
        #
        #  Read Arguments 
        #
        ########################################################################################
        $ExpectedNumberArguments = 3
        $Script:Arguments = $args.Clone()
        [System.Collections.ArrayList]$ArgumentList = [System.Collections.ArrayList]::new()
        0..$ExpectedNumberArguments |  % {
            $index = $_
            if([string]::IsNullOrEmpty($Script:Arguments[$index]) -eq $True){
             Write-Output "missing argument $index"
            }
            [void]$ArgumentList.Add("$($Script:Arguments[$index])")
        }

        [string]$Configuration          = $ArgumentList.Item(0)
        [string]$Platform               = $ArgumentList.Item(1)
        [string]$SolutionDirectory      = $ArgumentList.Item(2)
        [string]$BuiltBinary            = $ArgumentList.Item(3)

        [string]$SolutionDirectory      = (Resolve-Path "$SolutionDirectory").Path
        [string]$ScriptsDirectory       = Join-Path $SolutionDirectory 'scripts'
        [string]$IncludesPath           = Join-Path $SolutionDirectory 'include'
        [string]$IncludesMsgPackPath    = Join-Path $IncludesPath 'msgpack'
        [string]$FullIncludesPath       = Join-Path $IncludesPath 'msgpack\type\tr1'
        [string]$MD5HASH                = (Get-FileHash -Path "$BuiltBinary" -Algorithm MD5).Hash
        [string]$SourcePath             = Join-Path $SolutionDirectory 'src'
        [string]$MsgPackSrcPath         = Join-Path $SourcePath 'msgpack'
        [string]$MsgPackTypeSrcPath     = Join-Path $MsgPackSrcPath 'type'


        if(!(Test-Path "$BuiltBinary")){
            throw "ERROR: NO BINARY FOUND @`"$BuiltBinary`""
        }

        Write-Output "======================================================================================="
        Write-Output "                     MUA SERVER PROJECT POST BUILD OPERATIONS                          "
        Write-Output "======================================================================================="


    }catch{

        [System.Management.Automation.ErrorRecord]$Record=$_
        $Line = [string]::new('=',120)
        $Spaces = [string]::new(' ',49)
        $ErrorOccured=$True
        Write-Output "`n`n`n`n$Line`n$Spaces POST BUILD SCRIPT ERROR`n$Line"
        $formatstring = "{0} {1}"
        $fields = $Record.FullyQualifiedErrorId,$Record.Exception.ToString()
        $ExceptMsg=($formatstring -f $fields)
        $Stack=$Record.ScriptStackTrace
        Write-Output "$ExceptMsg"
        Write-Output "$Stack" 
        
        exit -9
    }   