% @file fft_test_devc.m
% IIR/FIR Filter implementation
% @author Ricardo <tsao.ricardo@iac.com.tw>

clear;

NFFT=1000;

% load raw data
fs=100; %sampling frequency
t=0:0.01:100; %time base

x = csvread('raw_data.csv');

%histogram(x);
%figure

std(x)
mean(x)

% plot raw data
subplot(2,2,1)
plot(t, x)
title('Raw Data')
xlabel('Time (s)')
ylabel('Angular Rate (\omega/s)');

% do FFT
X=fftshift(fft(x,NFFT));
fVals=fs*(-NFFT/2:NFFT/2-1)/NFFT;

% plot the FFT result of raw data
subplot(2,2,2)
plot(fVals,abs(X));	
title('Double Sided FFT - with FFTShift');
xlabel('Frequency (Hz)')
ylabel('DFT Values');

% load filtered data
x = csvread('filtered_data.csv');

% plot the filtered data
subplot(2,2,3)
plot(t, x)
title('Filtered Data (using devc++)')
xlabel('Time (s)');
ylabel('Angular Rate (\omega/s)');

% do FFT
X=fftshift(fft(x,NFFT));
fVals=fs*(-NFFT/2:NFFT/2-1)/NFFT;

% plot the FFT result of filtered data 
subplot(2,2,4)
plot(fVals,abs(X));	
title('Double Sided FFT - with FFTShift');
xlabel('Frequency(Hz)')	 	 
ylabel('DFT Values');

