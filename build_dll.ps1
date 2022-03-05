
$compiler = 'E:\x86_64-8.1.0-release-win32-seh-rt_v6-rev0\bin\gcc.exe'
$build_flags = '-v -flto -O3 -m64 -std=c99 -pedantic -s'
$link_flags = "-v '-Wl,--subsystem,windows'"

function Make-OFile( $ifile, $ofile, $flags ){
    Invoke-Expression "$compiler -c $ifile -o $ofile $Script:build_flags $flags"
    if( $LASTEXITCODE -ne 0 ){
        Write-Host "Compilation failed." -ForeGroundColor:Red
        exit
    }else{
        Write-Host "$ofile successfully compiled." -ForeGroundColor:Green
    }
}

function Link-DLL( $ifile, $ofile, $flags ){
    Invoke-Expression "$compiler -o $ofile -s -shared $ifile $Script:link_flags $flags"
    if( $LASTEXITCODE -ne 0 ){
        Write-Host "Linking failed." -ForeGroundColor:Red
        exit
    }else{
        Write-Host "$ofile successfully linked." -ForeGroundColor:Green
    }
}

function Make-Binary( $ifile, $ofile, $flags ){
    Invoke-Expression "$compiler $ifile -o $ofile $Script:build_flags $flags"
    if( $LASTEXITCODE -ne 0 ){
        Write-Host "Compilation failed." -ForeGroundColor:Red
        exit
    }else{
        Write-Host "$ofile successfully compiled." -ForeGroundColor:Green
    }
}

Clear-Host

Make-OFile -ifile 'cjson.c' -ofile 'cjson.o' -flags '-D CJSON_DLL_EXPORT'
Link-DLL -ifile 'cjson.o' -ofile 'cjson.dll'

