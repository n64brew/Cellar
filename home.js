function main()
{
    var date = new Date();
    var icon = document.getElementsByClassName("logo")[0].childNodes[1];
    switch (date.getMonth())
    {
        case 3:
            if (date.getDate() == 1)
                icon.src = "logo_fool.png";
            break;
        case 9:
            icon.src = "logospin_spooky.gif";
            break;
        case 11:
            icon.src = "logospin_xmas.gif";
            break;
    }
}

main()