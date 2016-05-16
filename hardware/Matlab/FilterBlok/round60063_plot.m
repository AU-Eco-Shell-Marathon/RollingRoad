function round60063_plot(X,ser)
% Plot a figure that compares component tolerances with rounding bin edges.
%
% (c) 2014 Stephen Cobeldick
%
% Creates a figure to illustrate how round60063's rounding bin edges
% closely match the component tolerances (these bin edge positions are
% modeled using the harmonic mean of adjacent ISO 60063 E-series values).
%
% Syntax:
%  round60063_plot(X,ser)
%
% See also ROUND60063 EQUIVCIRCUIT SIPRE SINUM BIPRE BINUM ROUND2SF ROUND2DP DATEROUND ROUND HISTC
%
% ### Example ###
%
% round60063_plot([1,10],'E6')
%
% ### Input Arguments ###
%
% Inputs:
%  X   = Numeric, some values to plot and view the rounding edges for.
%  ser = String, to select the E-Series. See "round60063.m" for a full list.
%
% round60063_plot(X,ser)

persistent fgh axh
%
% Get PNS and PNS-edge values:
[~,pns,edg,idx] = round60063(X,ser);
% Estimate tolerance (valid from E3 to E96):
tol = mean((pns(2:end)-pns(1:end-1))./(pns(2:end)+pns(1:end-1)));
pow = floor(log10(tol));
tol = round(tol*10^-pow)*10^pow;
%
% Re-use figure or create new figure:
if ishghandle(fgh)
    cla(axh);
else
    fgh = figure('NumberTitle','off','Name','Round60063 Demonstration','HandleVisibility','callback');
    axh = axes('Parent',fgh);
end
%
% Plot edges, PNS values, and tolerances:
idy = 1:numel(pns);
ln1 = plot(axh,[edg,edg],[1,numel(pns)],'g-');
hold(axh,'on')
ln2 = plot(axh,pns,idy,'db');
ln3 = plot(axh,[pns.'*(1-tol);pns.'*(1+tol)],[idy;idy],'or-');
ln4 = plot(axh,X(:),idx(:),'xk');
%
% Add labels and legend:
set(axh, 'YLim',idy([1,end]), 'YTick',idy, 'YTickLabel',pns)
xlabel(axh,'Input Values (pre-rounding)')
ylabel(axh,['Output Values (',ser,' Component Values)'])
legend(axh,[ln1(1),ln2(1),ln3(1),ln4(1)],'round60063.m Bin Edges','Nominal Component Value',...
    ['Component Tolerance (',num2str(100*tol),'%)'],'Input Matrix Values','Location','SouthEast')
title(axh,'Bin Edge and Component Tolerance Comparison for round60063.m')
%
end
%----------------------------------------------------------------------END:round60063_plot