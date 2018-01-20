function MatToBnd(d)
    %get contents
    directory = dir(d);
    index=1;
    for i = 4:size(directory,1)
        filePath = strcat(d, '/', directory(i).name)
        matFile = load(filePath);
        for j = 1:size(matFile.stereo, 2)
            points = matFile.stereo(j).pts_3d;
            outFile = filePath(1:end-4);
            if index < 10
                outFile = strcat(outFile,'_000',num2str(index));
            elseif index < 100
                outFile = strcat(outFile,'_00',num2str(index));
            elseif index < 1000
                outFile = strcat(outFile,'_0',num2str(index));
            else
                outFile = strcat(outFile, '_', num2str(index));
            end
            outFile = strcat(outFile, '.bndplus');
            dlmwrite(outFile, points);
            index = index+1;
        end
        index = 1;
    end
end