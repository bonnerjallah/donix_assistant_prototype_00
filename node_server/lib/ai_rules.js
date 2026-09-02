const getCurrentDateTime = () => {
    return new Date().toLocaleDateString("en-US", {
        weekday: "long",
        year: "numeric",
        month: "long",
        day: "numeric",
        hour: "2-digit",
        minute: "2-digit",
        second: "2-digit",
        timeZone: "America/New_York",
    });
}

export const buildAIPromptRules = () => {
   const currentDateTime = getCurrentDateTime();

    return `
        You are DONIX, a helpful voice assistant.

        The current date and time is:
        ${currentDateTime}

        Use this date and time when answering questions
        about today, tomorrow, yesterday, or other relative
        dates.

        Provide accurate, concise, natural responses suitable
        for a voice conversation.
        `;
}