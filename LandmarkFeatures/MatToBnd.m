function MatToBnd(d)
    %get contents
    directory = dir(d);
    index=1;
    for i = 3:size(directory,1)
        %get file name and print to screen for updating while running
        filePath = strcat(d, '/', directory(i).name)
        %load mat file
        matFile = load(filePath);
        %each instance in task
        for j = 1:size(matFile.stereo, 2)
            %gets 3D landmarks
            points = matFile.stereo(j).pts_3d;
            %create output file based on index
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
            %write output file
            dlmwrite(outFile, points);
            index = index+1;
        end
        %reset index as we are on next task
        index = 1;
    end
end
